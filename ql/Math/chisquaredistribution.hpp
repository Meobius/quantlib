/*
 Copyright (C) 2000, 2001, 2002, 2003 Sadruddin Rejeb

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
/*! \file chisquaredistribution.hpp
    \brief Chi-square (central and non-central) distributions

    \fullpath
    ql/Math/%chisquaredistribution.hpp
*/

// $Id$

#ifndef quantlib_chi_square_distribution_h
#define quantlib_chi_square_distribution_h

#include <ql/dataformatters.hpp>

namespace QuantLib {

    namespace Math {

        class ChiSquareDistribution
        : public std::unary_function<double,double> {
          public:
            ChiSquareDistribution(double df) : df_(df) {}
            double operator()(double x) const;
          private:
            double df_;
        };

        class NonCentralChiSquareDistribution
        : public std::unary_function<double,double> {
          public:
            NonCentralChiSquareDistribution(double df, double ncp) 
            : df_(df), ncp_(ncp) {}
            double operator()(double x) const;
          private:
            double df_, ncp_;
        };

    }

}


#endif
