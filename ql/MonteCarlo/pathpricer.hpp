
/*
 Copyright (C) 2003 Ferdinando Ametrano
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

/*! \file pathpricer.hpp
    \brief base class for single-path pricers
*/

// $Id$

#ifndef quantlib_montecarlo_path_pricer_h
#define quantlib_montecarlo_path_pricer_h

#include <ql/option.hpp>
#include <ql/types.hpp>
#include <ql/termstructure.hpp>
#include <functional>

namespace QuantLib {

    namespace MonteCarlo {

        //! base class for path pricers
        /*! Given a path the value of an option is returned on that path.
        */
        template<class PathType, class ValueType=double>
        class PathPricer : public std::unary_function<PathType, ValueType> {
          public:
            PathPricer(const RelinkableHandle<TermStructure>& riskFreeTS);
            virtual ~PathPricer() {}
            virtual ValueType operator()(const PathType& path) const=0;
          protected:
            RelinkableHandle<TermStructure> riskFreeTS_;
        };

        template<class P,class V>
        PathPricer<P,V>::PathPricer(
            const RelinkableHandle<TermStructure>& riskFreeTS)
       : riskFreeTS_(riskFreeTS) {
            QL_REQUIRE(!riskFreeTS.isNull(),
                       "PathPricer::PathPricer() : "
                       "no risk free term structure given");
        }



        //! base class for path pricers
        /*! Given a path the value of an option is returned on that path.

            \deprecated use PathPricer instead
        */
        template<class PathType, class ValueType=double>
        class PathPricer_old : public std::unary_function<PathType, ValueType> {
          public:
            PathPricer_old(DiscountFactor discount,
                       bool useAntitheticVariance);
            virtual ~PathPricer_old() {}
            virtual ValueType operator()(const PathType& path) const=0;
          protected:
            DiscountFactor discount_;
            bool useAntitheticVariance_;
        };

        template<class P,class V>
        PathPricer_old<P,V>::PathPricer_old(DiscountFactor discount,
            bool useAntitheticVariance)
            : discount_(discount),
              useAntitheticVariance_(useAntitheticVariance) {
            QL_REQUIRE(discount_ <= 1.0 && discount_ > 0.0,
                "PathPricer_old: discount must be positive");
        }

    }

}


#endif
