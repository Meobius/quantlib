
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

/*! \file blackvariancesurface.cpp
    \brief Black volatility surface modelled as variance surface
*/

#include <ql/Volatilities/blackvariancesurface.hpp>
#include <ql/Math/interpolationtraits.hpp>
#include <ql/dataformatters.hpp>

namespace QuantLib {

    using namespace Math;

    namespace VolTermStructures {

        BlackVarianceSurface::BlackVarianceSurface(
            const Date& referenceDate,
            const std::vector<Date>& dates,
            const std::vector<double>& strikes,
            const Matrix& blackVolMatrix,
            BlackVarianceSurface::Extrapolation lowerEx,
            BlackVarianceSurface::Extrapolation upperEx,
            const DayCounter& dayCounter)
        : referenceDate_(referenceDate), dayCounter_(dayCounter),
          maxDate_(dates.back()), strikes_(strikes),
          lowerExtrapolation_(lowerEx), upperExtrapolation_(upperEx) {

            QL_REQUIRE(dates.size()==blackVolMatrix.columns(),
                "BlackVarianceSurface::BlackVarianceSurface : "
                "mismatch between date vector and vol matrix colums");
            QL_REQUIRE(strikes_.size()==blackVolMatrix.rows(),
                "BlackVarianceSurface::BlackVarianceSurface : "
                "mismatch between money-strike vector and vol matrix rows");

            QL_REQUIRE(dates[0]>=referenceDate,
                "BlackVarianceSurface::BlackVarianceSurface : "
                "cannot have dates[0]<=referenceDate");

            variances_ = Matrix(strikes_.size(), dates.size());
            times_ = std::vector<Time>(dates.size());
            Size j, i;
            for (j=0; j<blackVolMatrix.columns(); j++) {
                times_[j] = dayCounter_.yearFraction(referenceDate, dates[j]);
                QL_REQUIRE(j==0 || times_[j]>times_[j-1],
                    "BlackVarianceSurface::BlackVarianceSurface : "
                    "dates must be sorted unique!");
                for (i=0; i<blackVolMatrix.rows(); i++) {
                    variances_[i][j] = times_[j] *
                        blackVolMatrix[i][j]*blackVolMatrix[i][j];
                    if (j==0) {
                        QL_REQUIRE(variances_[i][0]>0.0 || times_[0]==0.0,
                            "BlackVarianceCurve::BlackVarianceCurve : "
                            "variance must be positive");
                    } else {
                        QL_REQUIRE(variances_[i][j]>=variances_[i][j-1],
                            "BlackVarianceCurve::BlackVarianceCurve : "
                            "variance must be non-decreasing");
                    }
                }
            }
            // default: bilinear interpolation
            #if defined(QL_PATCH_MICROSOFT)
            setInterpolation(Linear());
            #else
            setInterpolation<Linear>();
            #endif
        }


        double 
        BlackVarianceSurface::blackVarianceImpl(Time t, double strike, 
                                                bool extrapolate) const {

            // it doesn't check if extrapolation is performed/allowed
            if (t==0.0) return 0.0;

            // enforce constant extrapolation when required
            if (strike < strikes_.front() && strike < strikes_.back()
                && extrapolate
                && lowerExtrapolation_ == ConstantExtrapolation)
                strike = strikes_.front();
            if (strike > strikes_.back() && strike > strikes_.front()
                && extrapolate
                && upperExtrapolation_ == ConstantExtrapolation)
                strike = strikes_.back();

            QL_REQUIRE(t>=0.0,
                "BlackVarianceSurface::blackVarianceImpl : "
                "negative time (" + DoubleFormatter::toString(t) +
                ") not allowed");
            if (t<=times_[0])
                return (*varianceSurface_)(times_[0], strike, extrapolate)*
                    t/times_[0];
            else if (t<=times_.back())
                return (*varianceSurface_)(t, strike, extrapolate);
            else // t>times_.back() || extrapolate
                QL_REQUIRE(extrapolate,
                    "ConstantVol::blackVolImpl : "
                    "time (" + DoubleFormatter::toString(t) +
                    ") greater than max time (" +
                    DoubleFormatter::toString(times_.back()) +
                    ")");
                return (*varianceSurface_)(times_.back(), strike, extrapolate)*
                    t/times_.back();
        }

    }

}

