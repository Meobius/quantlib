
/*
 Copyright (C) 2002, 2003 Ferdinando Ametrano
 Copyright (C) 2003 StatPro Italia srl

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

/*! \file blackvariancecurve.cpp
    \brief Black volatility curve modelled as variance curve
*/

#include <ql/Volatilities/blackvariancecurve.hpp>
#include <ql/Math/interpolationtraits.hpp>
#include <ql/dataformatters.hpp>

namespace QuantLib {

    using namespace Math;

    namespace VolTermStructures {

        BlackVarianceCurve::BlackVarianceCurve(
            const Date& referenceDate,
            const std::vector<Date>& dates,
            const std::vector<double>& blackVolCurve,
            const DayCounter& dayCounter)
        : referenceDate_(referenceDate), dayCounter_(dayCounter),
          maxDate_(dates.back()) {

            QL_REQUIRE(dates.size()==blackVolCurve.size(),
                "BlackVarianceCurve::BlackVarianceCurve : "
                "mismatch between date vector and black vol vector");

            // cannot have dates[0]==referenceDate, since the
            // value of the vol at dates[0] would be lost
            // (variance at referenceDate must be zero)
            QL_REQUIRE(dates[0]>referenceDate,
                "BlackVarianceCurve::BlackVarianceCurve : "
                "cannot have dates[0]<=referenceDate");

            variances_ = std::vector<double>(dates.size());
            times_ = std::vector<Time>(dates.size());
            Size j;
            for (j=0; j<blackVolCurve.size(); j++) {
                times_[j] = dayCounter_.yearFraction(referenceDate, dates[j]);
                QL_REQUIRE(j==0 || times_[j]>times_[j-1],
                    "BlackVarianceCurve::BlackVarianceCurve : "
                    "dates must be sorted unique!");
                variances_[j] = times_[j] *
                    blackVolCurve[j]*blackVolCurve[j];
                if (j==0) QL_REQUIRE(variances_[0]>0.0,
                    "BlackVarianceCurve::BlackVarianceCurve : "
                    "variance must be positive");
                if (j>0) QL_REQUIRE(variances_[j]>=variances_[j-1],
                    "BlackVarianceCurve::BlackVarianceCurve : "
                    "variance must be and non-decreasing");
            }

            // default: linear interpolation
            #if defined(QL_PATCH_MICROSOFT)
            setInterpolation(Linear());
            #else
            setInterpolation<Linear>();
            #endif
        }


        double BlackVarianceCurve::blackVarianceImpl(Time t, double, 
                                                     bool extrapolate) const {

            QL_REQUIRE(t>=0.0,
                "BlackVarianceCurve::blackVarianceImpl :"
                "negative time (" + DoubleFormatter::toString(t) +
                ") not allowed");

            // for early times extrapolate with flat vol
            if (t<=times_[0])
                return (*varianceCurve_)(times_[0], extrapolate)*
                    t/times_[0];
            else if (t<=times_.back())
                return (*varianceCurve_)(t, extrapolate);
            // for later times extrapolate with flat vol
            else { // t>times_.back() || extrapolate
                QL_REQUIRE(extrapolate,
                    "ConstantVol::blackVolImpl : "
                    "time (" + DoubleFormatter::toString(t) +
                    ") greater than max time (" +
                    DoubleFormatter::toString(times_.back()) +
                    ")");
                return (*varianceCurve_)(times_.back(), extrapolate)*
                    t/times_.back();
            }
        }

    }

}

