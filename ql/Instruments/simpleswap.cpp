
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

/*! \file simpleswap.cpp
    \brief Simple fixed-rate vs Libor swap
*/

#include <ql/Instruments/simpleswap.hpp>
#include <ql/CashFlows/cashflowvectors.hpp>
#include <ql/CashFlows/fixedratecoupon.hpp>
#include <ql/CashFlows/floatingratecoupon.hpp>

namespace QuantLib {

    namespace Instruments {

        SimpleSwap::SimpleSwap(
              bool payFixedRate,
              const Date& startDate, int n, TimeUnit units,
              const Calendar& calendar,
              RollingConvention rollingConvention,
              double nominal,
              int fixedFrequency,
              Rate fixedRate,
              bool fixedIsAdjusted,
              const DayCounter& fixedDayCount,
              int floatingFrequency,
              const Handle<Xibor>& index,
              int indexFixingDays,
              Spread spread,
              const RelinkableHandle<TermStructure>& termStructure,
              const std::string& isinCode, 
              const std::string& description)
        : Swap(std::vector<Handle<CashFlow> >(),
               std::vector<Handle<CashFlow> >(),
               termStructure, isinCode, description),
          payFixedRate_(payFixedRate), fixedRate_(fixedRate), spread_(spread), 
          nominal_(nominal) {

            Date maturity = calendar.roll(startDate.plus(n,units),
                                          rollingConvention);

            Schedule fixedSchedule = 
                MakeSchedule(calendar,startDate,maturity,
                             fixedFrequency,rollingConvention,
                             fixedIsAdjusted);
            Schedule floatSchedule =
                MakeSchedule(calendar,startDate,maturity,
                             floatingFrequency,rollingConvention,
                             true);

            std::vector<Handle<CashFlow> > fixedLeg =
                FixedRateCouponVector(fixedSchedule, 
                                      std::vector<double>(1,nominal), 
                                      std::vector<Rate>(1,fixedRate), 
                                      fixedDayCount);
            std::vector<Handle<CashFlow> > floatingLeg =
                FloatingRateCouponVector(floatSchedule,
                                         std::vector<double>(1,nominal),
                                         index, indexFixingDays, 
                                         std::vector<Spread>(1,spread));
            std::vector<Handle<CashFlow> >::const_iterator i;
            for (i = floatingLeg.begin(); i < floatingLeg.end(); ++i)
                registerWith(*i);

            if (payFixedRate_) {
                firstLeg_ = fixedLeg;
                secondLeg_ = floatingLeg;
            } else {
                firstLeg_ = floatingLeg;
                secondLeg_ = fixedLeg;
            }
        }

        SimpleSwap::SimpleSwap(
                bool payFixedRate,
                double nominal,
                const Schedule& fixedSchedule,
                Rate fixedRate,
                const DayCounter& fixedDayCount,
                const Schedule& floatSchedule,
                const Handle<Xibor>& index,
                int indexFixingDays,
                Spread spread,
                const RelinkableHandle<TermStructure>& termStructure,
                const std::string& isinCode, const std::string& description)
        : Swap(std::vector<Handle<CashFlow> >(),
               std::vector<Handle<CashFlow> >(),
               termStructure, isinCode, description),
          payFixedRate_(payFixedRate), fixedRate_(fixedRate), spread_(spread), 
          nominal_(nominal) {

            std::vector<Handle<CashFlow> > fixedLeg =
                FixedRateCouponVector(fixedSchedule,
                                      std::vector<double>(1,nominal), 
                                      std::vector<Rate>(1,fixedRate), 
                                      fixedDayCount);
            std::vector<Handle<CashFlow> > floatingLeg =
                FloatingRateCouponVector(floatSchedule,
                                         std::vector<double>(1,nominal),
                                         index, indexFixingDays, 
                                         std::vector<Spread>(1,spread));
            std::vector<Handle<CashFlow> >::const_iterator i;
            for (i = floatingLeg.begin(); i < floatingLeg.end(); ++i)
                registerWith(*i);

            if (payFixedRate_) {
                firstLeg_ = fixedLeg;
                secondLeg_ = floatingLeg;
            } else {
                firstLeg_ = floatingLeg;
                secondLeg_ = fixedLeg;
            }
        }


        void SimpleSwap::setupArguments(Arguments* args) const {
            SimpleSwap::arguments* arguments =
                dynamic_cast<SimpleSwap::arguments*>(args);

            QL_REQUIRE(arguments != 0, 
                       "SimpleSwap::setupArguments : "
                       "wrong argument type");

            arguments->payFixed = payFixedRate_;
            arguments->nominal = nominal_;

            Date settlement = termStructure_->referenceDate();
            DayCounter counter = termStructure_->dayCounter();
            Size i;

            const std::vector<Handle<CashFlow> >& fixedCoupons = fixedLeg();

            arguments->fixedResetTimes = arguments->fixedPayTimes =
                std::vector<Time>(fixedCoupons.size());
            arguments->fixedCoupons = std::vector<double>(fixedCoupons.size());

            for (i=0; i<fixedCoupons.size(); i++) {
                Handle<FixedRateCoupon> coupon = fixedCoupons[i];

                Time time = counter.yearFraction(settlement, coupon->date());
                arguments->fixedPayTimes[i] = time;
                time = counter.yearFraction(settlement, 
                                            coupon->accrualStartDate());
                arguments->fixedResetTimes[i] = time;
                arguments->fixedCoupons[i] = coupon->amount();
            }

            const std::vector<Handle<CashFlow> >& floatingCoupons = 
                floatingLeg();

            arguments->floatingResetTimes = arguments->floatingPayTimes =
                arguments->floatingAccrualTimes = 
                    std::vector<Time>(floatingCoupons.size());
            arguments->floatingSpreads =
                std::vector<Spread>(floatingCoupons.size());

            for (i=0; i<floatingCoupons.size(); i++) {
                Handle<FloatingRateCoupon> coupon = floatingCoupons[i];
                Date resetDate = coupon->accrualStartDate(); // already rolled
                Time time = counter.yearFraction(settlement, resetDate);
                arguments->floatingResetTimes[i] = time;
                time = counter.yearFraction(settlement, coupon->date());
                arguments->floatingPayTimes[i] = time;
                arguments->floatingAccrualTimes[i] = coupon->accrualPeriod();
                arguments->floatingSpreads[i] = coupon->spread();
            }
        }


        void SimpleSwap::arguments::validate() const {
            QL_REQUIRE(nominal != Null<double>(),
                       "SimpleSwap::arguments: "
                       "nominal null or not set");
            QL_REQUIRE(fixedResetTimes.size() == fixedPayTimes.size(), 
                       "SimpleSwap::arguments: "
                       "number of fixed start times different from "
                       "number of fixed payment times");
            QL_REQUIRE(fixedPayTimes.size() == fixedCoupons.size(), 
                       "SimpleSwap::arguments: "
                       "number of fixed payment times different from "
                       "number of fixed coupon amounts");
            QL_REQUIRE(floatingResetTimes.size() == floatingPayTimes.size(), 
                       "SimpleSwap::arguments: "
                       "number of floating start times different from "
                       "number of floating payment times");
            QL_REQUIRE(floatingAccrualTimes.size() == floatingPayTimes.size(), 
                       "SimpleSwap::arguments: "
                       "number of floating accrual times different from "
                       "number of floating payment times");
            QL_REQUIRE(floatingSpreads.size() == floatingPayTimes.size(), 
                       "SimpleSwap::arguments: "
                       "number of floating spreads different from "
                       "number of floating payment times");
        }

    }

}

