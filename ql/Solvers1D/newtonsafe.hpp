
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file newtonsafe.hpp
    \brief Safe (bracketed) Newton 1-D solver
*/

#ifndef quantlib_solver1d_newtonsafe_h
#define quantlib_solver1d_newtonsafe_h

#include <ql/solver1d.hpp>

namespace QuantLib {

    //! safe %Newton 1-D solver
    /*! \note This solver requires that the passed function object
              implement a method <tt>double derivative(double)</tt>.
    */
    class NewtonSafe : public Solver1D<NewtonSafe> {
      public:
        template <class F>
        double solveImpl(const F& f, double xAccuracy) const {

            /* The implementation of the algorithm was inspired by
               Press, Teukolsky, Vetterling, and Flannery,
               "Numerical Recipes in C", 2nd edition, 
               Cambridge University Press
            */

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
                if ((((root_-xh)*dfroot-froot)*
                     ((root_-xl)*dfroot-froot) > 0.0)
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

            throw Error("NewtonSafe::solveImpl "
                        "maximum number of function evaluations (" +
                        IntegerFormatter::toString(maxEvaluations_) +
                        ") exceeded");

            QL_DUMMY_RETURN(0.0);
        }
    };

}


#endif
