/*
 Copyright (C) 2000, 2001, 2002 Sadruddin Rejeb

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
/*! \file gammadistribution.hpp
    \brief Gamma distribution

    \fullpath
    ql/Math/%gammadistribution.hpp
*/

// $Id$

#ifndef quantlib_math_gamma_distribution_h
#define quantlib_math_gamma_distribution_h

#include <ql/errors.hpp>
#include <ql/types.hpp>

#include <functional>

namespace QuantLib {

    namespace Math {

        class GammaDistribution
        : public std::unary_function<double,double> {
          public:
            GammaDistribution(double a) : a_(a) {
                QL_REQUIRE(a>0.0, "Invalid parameter for gamma distribution");
            }
            double operator()(double x) const;
          private:
            double a_;
        };

        //! Gamma function class
        /*! This is a function defined by 
            \f[
                \Gamma(z) = \int_0^{\infty}t^{z-1}e^{-t}dt
            \f]
        */
        class GammaFunction : public std::unary_function<double,double> {
          public:
            double logValue(double x) const;
          private:
            static const double c1_, c2_, c3_, c4_, c5_, c6_;
        };

    }

}


#endif
