
/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file capfloor.cpp
    \brief European cap and floor class
*/

#include <ql/CashFlows/floatingratecoupon.hpp>
#include <ql/Pricers/blackcapfloor.hpp>
#include <ql/Solvers1D/brent.hpp>

namespace QuantLib {

    using Pricers::BlackCapFloor;

    CapFloor::CapFloor(CapFloor::Type type,
                       const std::vector<Handle<CashFlow> >& floatingLeg,
                       const std::vector<Rate>& capRates,
                       const std::vector<Rate>& floorRates,
                       const RelinkableHandle<TermStructure>& termStructure,
                       const Handle<PricingEngine>& engine)
    : type_(type), floatingLeg_(floatingLeg),
      capRates_(capRates), floorRates_(floorRates),
      termStructure_(termStructure) {
        setPricingEngine(engine);
        if (type_ == Cap || type_ == Collar) {
            QL_REQUIRE(!capRates_.empty(),
                       "CapFloor: no cap rates given");
            while (capRates_.size() < floatingLeg_.size())
                capRates_.push_back(capRates_.back());
        }
        if (type_ == Floor || type_ == Collar) {
            QL_REQUIRE(!floorRates_.empty(),
                       "CapFloor: no floor rates given");
            while (floorRates_.size() < floatingLeg_.size())
                floorRates_.push_back(floorRates_.back());
        }
        std::vector<Handle<CashFlow> >::const_iterator i;
        for (i = floatingLeg_.begin(); i != floatingLeg_.end(); ++i)
            registerWith(*i);
        registerWith(termStructure);
    }

    bool CapFloor::isExpired() const {
        Date lastFixing = Date::minDate();
        for (Size i=0; i<floatingLeg_.size(); i++)
            lastFixing = QL_MAX(lastFixing, floatingLeg_[i]->date());
        return lastFixing < termStructure_->referenceDate();
    }

    void CapFloor::setupArguments(Arguments* args) const {
        CapFloor::arguments* arguments =
            dynamic_cast<CapFloor::arguments*>(args);
        QL_REQUIRE(arguments != 0,
                   "CapFloor::setupArguments :"
                   "wrong argument type");

        arguments->type = type_;
        arguments->capRates.clear();
        arguments->floorRates.clear();
        arguments->startTimes.clear();
        arguments->fixingTimes.clear();
        arguments->endTimes.clear();
        arguments->accrualTimes.clear();
        arguments->forwards.clear();
        arguments->nominals.clear();

        Date today = termStructure_->todaysDate();
        Date settlement = termStructure_->referenceDate();
        DayCounter counter = termStructure_->dayCounter();

        for (Size i=0; i<floatingLeg_.size(); i++) {
            Handle<FloatingRateCoupon> coupon = floatingLeg_[i];
            Date beginDate = coupon->accrualStartDate();
            Time time = counter.yearFraction(settlement, beginDate);
            arguments->startTimes.push_back(time);
            Date fixingDate = coupon->fixingDate();
            time = counter.yearFraction(today, fixingDate);
            arguments->fixingTimes.push_back(time);
            time = counter.yearFraction(settlement, coupon->date());
            arguments->endTimes.push_back(time);
            // this is passed explicitly for precision
            arguments->accrualTimes.push_back(coupon->accrualPeriod());
            // this is passed explicitly for precision
            if (arguments->endTimes.back() >= 0.0)  // but only if needed 
                arguments->forwards.push_back(coupon->fixing());
            else
                arguments->forwards.push_back(Null<Rate>());
            arguments->nominals.push_back(coupon->nominal());
            if (type_ == Cap || type_ == Collar)
                arguments->capRates.push_back(capRates_[i]);
            if (type_ == Floor || type_ == Collar)
                arguments->floorRates.push_back(floorRates_[i]);
        }
    }

    void CapFloor::arguments::validate() const {
        QL_REQUIRE(endTimes.size() == startTimes.size(),
                   "Invalid pricing arguments: size of startTimes (" +
                   IntegerFormatter::toString(startTimes.size()) +
                   ") different from that of endTimes (" +
                   IntegerFormatter::toString(endTimes.size()) +
                   ")");
        QL_REQUIRE(accrualTimes.size() == startTimes.size(),
                   "Invalid pricing arguments: size of startTimes (" +
                   IntegerFormatter::toString(startTimes.size()) +
                   ") different from that of accrualTimes (" +
                   IntegerFormatter::toString(accrualTimes.size()) +
                   ")");
        QL_REQUIRE(type == CapFloor::Floor || 
                   capRates.size() == startTimes.size(),
                   "Invalid pricing arguments: size of startTimes (" +
                   IntegerFormatter::toString(startTimes.size()) +
                   ") different from that of capRates (" +
                   IntegerFormatter::toString(capRates.size()) +
                   ")");
        QL_REQUIRE(type == CapFloor::Cap ||
                   floorRates.size() == startTimes.size(),
                   "Invalid pricing arguments: size of startTimes (" +
                   IntegerFormatter::toString(startTimes.size()) +
                   ") different from that of floorRates (" +
                   IntegerFormatter::toString(floorRates.size()) +
                   ")");
        QL_REQUIRE(nominals.size() == startTimes.size(),
                   "Invalid pricing arguments: size of startTimes (" +
                   IntegerFormatter::toString(startTimes.size()) +
                   ") different from that of nominals (" +
                   IntegerFormatter::toString(nominals.size()) +
                   ")");
    }

    double CapFloor::impliedVolatility(double targetValue,
                                       double accuracy, 
                                       Size maxEvaluations,
                                       double minVol, 
                                       double maxVol) const {
        calculate();
        QL_REQUIRE(!isExpired(),
                   "CapFloor::impliedVolatility : instrument expired");

        double guess = 0.10;   // no way we can get a more accurate one

        ImpliedVolHelper f(*this,termStructure_,targetValue);
        Solvers1D::Brent solver;
        solver.setMaxEvaluations(maxEvaluations);
        return solver.solve(f, accuracy, guess, minVol, maxVol);
    }


    CapFloor::ImpliedVolHelper::ImpliedVolHelper(
                         const CapFloor& cap, 
                         const RelinkableHandle<TermStructure>& termStructure,
                         double targetValue)
    : termStructure_(termStructure), targetValue_(targetValue) {

        vol_ = Handle<SimpleMarketElement>(new SimpleMarketElement(0.0));
        RelinkableHandle<MarketElement> h(vol_);
        Handle<BlackModel> model(new BlackModel(h,termStructure_));
        engine_ = Handle<PricingEngine>(new BlackCapFloor(model));

        cap.setupArguments(engine_->arguments());

        results_ = dynamic_cast<const Value*>(engine_->results());
    }

    double CapFloor::ImpliedVolHelper::operator()(double x) const {
        vol_->setValue(x);
        engine_->calculate();
        return results_->value-targetValue_;
    }

}
