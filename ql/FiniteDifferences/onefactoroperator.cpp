

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
/*! \file onefactoroperator.cpp
    \brief differential operator for one-factor interest rate models

    \fullpath
    ql/FiniteDifferences/%onefactoroperator.cpp
*/

// $Id$

#include "ql/FiniteDifferences/onefactoroperator.hpp"

namespace QuantLib {

    namespace FiniteDifferences {

        OneFactorOperator::OneFactorOperator(const Array& grid,
            const Handle<ShortRateProcess>& process)
        : TridiagonalOperator(grid.size()) {
             timeSetter_ = Handle<TridiagonalOperator::TimeSetter>(
               new SpecificTimeSetter(grid[0], grid[1] - grid[0], process));
         }

        OneFactorOperator::SpecificTimeSetter::SpecificTimeSetter(double x0,
            double dx, const Handle<ShortRateProcess>& process)
        : x0_(x0), dx_(dx), process_(process) {}

        void OneFactorOperator::SpecificTimeSetter::setTime(Time t,
            TridiagonalOperator& op) const {
            Size length = op.size();
            for (Size i=0; i<length; i++) {
                double x = x0_ + dx_*i;

                Rate r = process_->shortRate(x, t);
                double mu = process_->drift(x, t);
                double sigma = process_->diffusion(x, t);

                double sigma2 = sigma*sigma;
                double pdown = (- sigma2/(2.0*dx_*dx_) ) + mu/(2.0*dx_);
                double pm    = (+ sigma2/(dx_*dx_) )     + r;
                double pup   = (- sigma2/(2.0*dx_*dx_) ) - mu/(2.0*dx_);
                if (i==0)
                    op.setFirstRow(pm, pup);
                else if (i==(length - 1))
                    op.setLastRow(pdown, pm);
                else
                    op.setMidRow(i, pdown, pm, pup);
            }
            //setLowerBC( BoundaryCondition(BoundaryCondition::Neumann, 0));
            //setUpperBC( BoundaryCondition(BoundaryCondition::Neumann, 0));
        }
    }
}
