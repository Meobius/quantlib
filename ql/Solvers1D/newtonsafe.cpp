
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

/*! \file newtonsafe.cpp
    \brief Safe (braketed) Newton 1-D solver
*/

/* The implementation of the algorithm was inspired by
   Press, Teukolsky, Vetterling, and Flannery,
   "Numerical Recipes in C", 2nd edition, 
   Cambridge University Press
*/


#include <ql/Solvers1D/newtonsafe.hpp>

namespace QuantLib {

    namespace Solvers1D {

        double NewtonSafe::solve_(const ObjectiveFunction& f,
                                  double xAccuracy) const {

            double froot, dfroot, dx, dxold;
            double xh, xl;

            // Orient the search so that f(xl) < 0
            if (fxMin_ < 0.0) {
                xl=xMin_;
                xh=xMax_;
            } else {
                xh=xMin_;
                xl=xMax_;
            }

            // the "stepsize before last"
            dxold=xMax_-xMin_;
            // it was dxold=QL_FABS(xMax_-xMin_); in Numerical Recipes
            // here (xMax_-xMin_ > 0) is verified in the constructor

            // and the last step
            dx=dxold;

            froot = f(root_);
            dfroot = f.derivative(root_);
            QL_REQUIRE(dfroot != Null<double>(),
                "NewtonSafe requires function's derivative");
            evaluationNumber_++;

            while (evaluationNumber_<=maxEvaluations_) {
                // Bisect if (out of range || not decreasing fast enough)
                if ((((root_-xh)*dfroot-froot)*((root_-xl)*dfroot-froot) > 0.0)
                        || (QL_FABS(2.0*froot) > QL_FABS(dxold*dfroot))) {
                    dxold = dx;
                    dx = (xh-xl)/2.0;
                    root_=xl+dx;
                } else {
                    dxold=dx;
                    dx=froot/dfroot;
                    root_ -= dx;
                }
                // Convergence criterion
                if (QL_FABS(dx) < xAccuracy)
                    return root_;
                froot = f(root_);
                dfroot = f.derivative(root_);
                evaluationNumber_++;
                if (froot < 0.0)
                    xl=root_;
                else
                    xh=root_;
            }

            throw Error("NewtonSafe: "
                        "maximum number of function evaluations (" +
                        IntegerFormatter::toString(maxEvaluations_) +
                        ") exceeded");

            QL_DUMMY_RETURN(0.0);

        }

    }

}
