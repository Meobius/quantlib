
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

/*! \file fdmultiperiodoption.cpp
    \brief base class for option with events happening at different periods
*/

#include <ql/Pricers/europeanoption.hpp>
#include <ql/FiniteDifferences/americancondition.hpp>
#include <ql/Pricers/fdmultiperiodoption.hpp>
#include <ql/FiniteDifferences/valueatcenter.hpp>

namespace QuantLib {

    namespace Pricers {

        FdMultiPeriodOption::FdMultiPeriodOption(Option::Type type,
            double underlying, double strike, Spread dividendYield,
            Rate riskFreeRate, Time residualTime, double volatility,
            int gridPoints, const std::vector<Time>& dates,
            int timeSteps)
        : FdBsmOption(type, underlying, strike,
                             dividendYield, riskFreeRate,
                             residualTime, volatility,
                             gridPoints),
          dates_(dates),
          dateNumber_(dates.size()),
          timeStepPerPeriod_(timeSteps),
          lastDateIsResTime_(false),
          lastIndex_(dateNumber_ - 1),
          firstDateIsZero_(false),
          firstNonZeroDate_(residualTime),
          firstIndex_(-1) {

            double dateTollerance = 1e-6;

            if (dateNumber_ > 0){
                QL_REQUIRE(dates_[0] >= 0,
                          "First date " +
                          DoubleFormatter::toString(dates_[0]) +
                          " cannot be negative");
                if(dates_[0] < residualTime * dateTollerance ){
                    firstDateIsZero_ = true;
                    firstIndex_ = 0;
                    if(dateNumber_ >= 2)
                        firstNonZeroDate_ = dates_[1];
                }

                if(QL_FABS(dates_[lastIndex_] - residualTime) < dateTollerance){
                    lastDateIsResTime_ = true;
                    lastIndex_ =dateNumber_ - 2;
                 }

                QL_REQUIRE(dates_[dateNumber_-1] <= residualTime,
                    "Last date, " +
                    DoubleFormatter::toString(dates_[dateNumber_-1]) +
                    ", must be within the residual time of " +
                    DoubleFormatter::toString(residualTime) );

                if (dateNumber_ >= 2){
                    if (!firstDateIsZero_)
                        firstNonZeroDate_ = dates_[0];
                    for (Size j = 1; j < dateNumber_; j++)
                        QL_REQUIRE(dates_[j-1] < dates_[j],
                            "Dates must be in increasing order:" +
                            DoubleFormatter::toString(dates_[j-1]) +
                            " is not strictly smaller than " +
                            DoubleFormatter::toString(dates_[j]) );
                }
            }

        }

        void FdMultiPeriodOption::calculate() const {

            Time beginDate, endDate;
            initializeControlVariate();
            setGridLimits(underlying_, residualTime_);
            initializeGrid();
            initializeInitialCondition();
            initializeOperator();
            initializeModel();
            initializeStepCondition();
            prices_ = intrinsicValues_;
            controlPrices_ = intrinsicValues_;

            if(lastDateIsResTime_)
                executeIntermediateStep(dateNumber_ - 1);

            double dt = residualTime_/(timeStepPerPeriod_*(dateNumber_+1));

            // Ensure that dt is always smaller than the first non-zero date
            if (firstNonZeroDate_ <= dt)
                dt = firstNonZeroDate_/2.0;

            int j = lastIndex_;
            do{
                if (j == int(dateNumber_) - 1)
                    beginDate = residualTime_;
                else
                    beginDate = dates_[j+1];

                if (j >= 0)
                    endDate = dates_[j];
                else
                    endDate = dt;

                model_ -> rollback(prices_, beginDate, endDate,
                                   timeStepPerPeriod_, stepCondition_);

                model_ -> rollback(controlPrices_, beginDate, endDate,
                                   timeStepPerPeriod_);

                if (j >= 0)
                    executeIntermediateStep(j);

            } while (--j >= firstIndex_);

            model_ -> rollback(prices_,        dt, 0, 1, stepCondition_);
            model_ -> rollback(controlPrices_, dt, 0, 1);

            if(firstDateIsZero_)
                executeIntermediateStep(0);

            // Option price and greeks are computed
            controlVariateCorrection_ =
                analytic_ -> value() - valueAtCenter(controlPrices_);

            value_ =   valueAtCenter(prices_)
                     - valueAtCenter(controlPrices_)
                     + analytic_ -> value();

            delta_ =   firstDerivativeAtCenter(prices_, grid_)
                     - firstDerivativeAtCenter(controlPrices_, grid_)
                     + analytic_ -> delta();

            gamma_ =   secondDerivativeAtCenter(prices_, grid_)
                     - secondDerivativeAtCenter(controlPrices_, grid_)
                     + analytic_ -> gamma();

            hasBeenCalculated_ = true;
        }

        void FdMultiPeriodOption::initializeControlVariate() const{
            analytic_ = Handle<SingleAssetOption> (new EuropeanOption (
                            payoff_.optionType(), underlying_, payoff_.strike(), dividendYield_,
                            riskFreeRate_, residualTime_, volatility_));
        }

        void FdMultiPeriodOption::initializeStepCondition() const{
            stepCondition_ = Handle<StandardStepCondition> (
                new AmericanCondition(intrinsicValues_));
        }

        void FdMultiPeriodOption::initializeModel() const{
            model_ = Handle<StandardFiniteDifferenceModel> (
                     new StandardFiniteDifferenceModel
                            (finiteDifferenceOperator_,BCs_));
        }

        double FdMultiPeriodOption::controlVariateCorrection() const{
            if(!hasBeenCalculated_)
                calculate();
            return controlVariateCorrection_;
        }

    }

}
