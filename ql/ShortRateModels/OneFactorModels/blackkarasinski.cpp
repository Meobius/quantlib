/*
 Copyright (C) 2001, 2002 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file blackkarasinski.cpp
    \brief Black-Karasinski model

    \fullpath
    ql/ShortRateModels/OneFactorModels/%blackkarasinski.cpp
*/

// $Id$

#include "ql/ShortRateModels/OneFactorModels/blackkarasinski.hpp"
#include "ql/Lattices/trinomialtree.hpp"
#include "ql/Solvers1D/brent.hpp"

namespace QuantLib {

    namespace ShortRateModels {

        using namespace Lattices;

        //Private function used by solver to determine time-dependent parameter
        class BlackKarasinski::Helper : public ObjectiveFunction {
          public:
            Helper(Size i, double xMin, double dx,
                   double discountBondPrice, 
                   const Handle<ShortRateTree>& tree)
            : size_(tree->size(i)), 
              dt_(tree->timeGrid().dt(i)),
              xMin_(xMin), dx_(dx),
              statePrices_(tree->statePrices(i)),
              discountBondPrice_(discountBondPrice) {}

            double operator()(double theta) const {
                double value = discountBondPrice_;
                double x = xMin_;
                for (Size j=0; j<size_; j++) {
                    double discount = QL_EXP(-QL_EXP(theta+x)*dt_);
                    value -= statePrices_[j]*discount;
                    x += dx_;
                }
                return value;
            }

          private:
            Size size_;
            Time dt_;
            double xMin_, dx_;
            const Array& statePrices_;
            double discountBondPrice_;
        };

        using Optimization::PositiveConstraint;

        BlackKarasinski::BlackKarasinski(
            const RelinkableHandle<TermStructure>& termStructure,
            double a, double sigma)
        : OneFactorModel(2), TermStructureConsistentModel(termStructure), 
          a_(arguments_[0]), sigma_(arguments_[1]) {
            a_ = ConstantParameter(a, PositiveConstraint());
            sigma_ = ConstantParameter(sigma, PositiveConstraint());
        }

        Handle<Lattice> BlackKarasinski::tree(
            const TimeGrid& grid) const {

            TermStructureFittingParameter phi(termStructure());

            Handle<ShortRateDynamics> numericDynamics(
                new Dynamics(phi, a(), sigma()));

            Handle<TrinomialTree> trinomial(
                new TrinomialTree(numericDynamics->process(), grid));
            Handle<ShortRateTree> numericTree(
                new ShortRateTree(trinomial, numericDynamics, grid));

            Handle<TermStructureFittingParameter::NumericalImpl> impl = 
                phi.implementation();
            impl->reset();
            double value = 1.0;
            double vMin = -50.0;
            double vMax = 50.0;
            for (Size i=0; i<(grid.size() - 1); i++) {
                double discountBond = termStructure()->discount(grid[i+1]);
                double xMin = trinomial->underlying(i, 0);
                double dx = trinomial->dx(i);
                Helper finder(i, xMin, dx, discountBond, numericTree);
                Solvers1D::Brent s1d = Solvers1D::Brent();
                s1d.setMaxEvaluations(1000);
                value = s1d.solve(finder, 1e-7, value, vMin, vMax);
                impl->set(grid[i], value);
//                vMin = value - 10.0;
//                vMax = value + 10.0;
            }
            return numericTree;
        }

    }

}
