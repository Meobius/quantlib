
/*
 Copyright (C) 2000, 2001, 2002 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software developed by the QuantLib Group; you can
 redistribute it and/or modify it under the terms of the QuantLib License;
 either version 1.0, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 QuantLib License for more details.

 You should have received a copy of the QuantLib License along with this
 program; if not, please email ferdinando@ametrano.net

 The QuantLib License is also available at http://quantlib.org/license.html
 The members of the QuantLib Group are listed in the QuantLib License
*/
/*! \file armijo.cpp
    \brief Armijo line-search class

    \fullpath
    ql/Optimization/%armijo.cpp
*/

#include "ql/Optimization/armijo.hpp"

namespace QuantLib {

    namespace Optimization {

        double ArmijoLineSearch::operator()(
            OptimizationProblem &P, // Optimization problem
            double t_ini,           // initial value of line-search step
            double q0,              // function value
            double qp0)             // squared norm of gradient vector
        {
            bool maxIter = false;
            qt_ = q0;
            qpt_ = qp0;
            double qtold, t = t_ini;
            int loopNumber = 0;

            OptimizationMethod &method = P.optimisationMethod ();
            Array & x = method.x ();
            Array & d = method.searchDirection ();

            // Initialize gradient
            gradient_ = Array (x.size ());
            // Compute new point
            xtd_ = x + t * d;
            // Compute function value at the new point
            qt_ = P.value (xtd_);

            // Enter in the loop if the criterion is not satisfied
            if ((qt_ - q0) > -alpha_ * t * qpt_) {
                do {
                    loopNumber++;
                    // Decrease step
                    t *= beta_;
                    // Store old value of the function
                    qtold = qt_;
                    // New point value
                    xtd_ = x + t * d;
                    // Compute function value at the new point
                    qt_ = P.value (xtd_);
                    P.gradient (gradient_, xtd_);
                    // and it squared norm
                    maxIter = P.optimisationMethod ().endCriteria ().
                        checkIterationNumber (loopNumber);
                } while (
                    (((qt_ - q0) > (-alpha_ * t * qpt_)) ||
                    ((qtold - q0) <= (-alpha_ * t * qpt_ / beta_))) &&
                    (!maxIter));
            }

            if (maxIter)
                succeed_ = false;

            // Compute new gradient
            P.gradient(gradient_, xtd_);
            // and it squared norm
            qpt_ = DotProduct(gradient_, gradient_);

            // Return new step value
            return t;
        }

    }

}
