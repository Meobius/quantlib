
/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file onefactormodel.cpp
    \brief Abstract one-factor interest rate model class
*/

#include "ql/ShortRateModels/onefactormodel.hpp"
#include "ql/Lattices/trinomialtree.hpp"
#include "ql/Solvers1D/brent.hpp"

namespace QuantLib {

    namespace ShortRateModels {

        using namespace Lattices;

        //Private function used by solver to determine time-dependent parameter
        class OneFactorModel::ShortRateTree::Helper {
          public:
            Helper(
                Size i,
                double discountBondPrice,
                const Handle<TermStructureFittingParameter::NumericalImpl>& 
                      theta,
                ShortRateTree& tree)
            : size_(tree.size(i)),
              i_(i),
              statePrices_(tree.statePrices(i)),
              discountBondPrice_(discountBondPrice),
              theta_(theta),
              tree_(tree) {
                theta_->set(tree.timeGrid()[i], 0.0);
            }

            double operator()(double theta) const {
                double value = discountBondPrice_;
                theta_->change(theta);
                for (Size j=0; j<size_; j++)
                    value -= statePrices_[j]*tree_.discount(i_,j);
                return value;
            }

          private:
            Size size_;
            Size i_;
            const Array& statePrices_;
            double discountBondPrice_;
            Handle<TermStructureFittingParameter::NumericalImpl> theta_;
            ShortRateTree& tree_;
        };

        OneFactorModel::ShortRateTree::ShortRateTree(
            const Handle<Tree>& tree,
            const Handle<ShortRateDynamics>& dynamics,
            const Handle<TermStructureFittingParameter::NumericalImpl>& theta,
            const TimeGrid& timeGrid)
        : Lattice(timeGrid, tree->size(1)), tree_(tree), dynamics_(dynamics) {

            theta->reset();
            double value = 1.0;
            double vMin = -100.0;
            double vMax = 100.0;
            for (Size i=0; i<(timeGrid.size() - 1); i++) {
                double discountBond = theta->termStructure()->discount(t_[i+1]);
                Helper finder(i, discountBond, theta, *this);
                Solvers1D::Brent s1d = Solvers1D::Brent();
                s1d.setMaxEvaluations(1000);
                value = s1d.solve(finder, 1e-7, value, vMin, vMax);
//                vMin = value - 1.0;
//                vMax = value + 1.0;
                theta->change(value);
            }
        }

        OneFactorModel::ShortRateTree::ShortRateTree(
            const Handle<Tree>& tree,
            const Handle<ShortRateDynamics>& dynamics,
            const TimeGrid& timeGrid)
        : Lattice(timeGrid, tree->size(1)), tree_(tree), dynamics_(dynamics) {}

        OneFactorModel::OneFactorModel(Size nArguments) : Model(nArguments) {}

        Handle<Lattice> OneFactorModel::tree(const TimeGrid& grid) const {
            Handle<Tree> trinomial(
                new TrinomialTree(dynamics()->process(), grid));
            return Handle<Lattice>(
                new ShortRateTree(trinomial, dynamics(), grid));
        }

    }

}
