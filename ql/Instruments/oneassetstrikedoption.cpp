
/*
 Copyright (C) 2003 Ferdinando Ametrano
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

/*! \file oneassetstrikedoption.cpp
    \brief Option on a single asset with striked payoff
*/

#include <ql/Instruments/oneassetstrikedoption.hpp>
#include <ql/Instruments/payoffs.hpp>

namespace QuantLib {

    OneAssetStrikedOption::OneAssetStrikedOption(
        const Handle<BlackScholesStochasticProcess>& stochProc,
        const Handle<StrikedTypePayoff>& payoff,
        const Handle<Exercise>& exercise,
        const Handle<PricingEngine>& engine,
        const std::string& isinCode,
        const std::string& description)
    : OneAssetOption(stochProc, payoff, exercise, engine, isinCode,
      description) {}


    double OneAssetStrikedOption::strikeSensitivity() const {
        calculate();
        QL_REQUIRE(strikeSensitivity_ != Null<double>(),
                   "OneAssetStrikedOption: strike sensitivity not provided");
        return strikeSensitivity_;
    }

    void OneAssetStrikedOption::setupArguments(Arguments* args) const {

        OneAssetStrikedOption::arguments* moreArgs =
            dynamic_cast<OneAssetStrikedOption::arguments*>(args);
        QL_REQUIRE(moreArgs != 0,
                   "OneAssetStrikedOption::setupArguments : "
                   "wrong argument type");
        moreArgs->payoff = payoff_;

        OneAssetOption::arguments* arguments =
            dynamic_cast<OneAssetOption::arguments*>(args);
        QL_REQUIRE(arguments != 0,
                   "OneAssetStrikedOption::setupArguments : "
                   "wrong argument type");
        OneAssetOption::setupArguments(arguments);

    }


    void OneAssetStrikedOption::performCalculations() const {
        OneAssetOption::performCalculations();
        const MoreGreeks* results =
            dynamic_cast<const MoreGreeks*>(engine_->results());
        QL_ENSURE(results != 0,
                  "OneAssetStrikedOption::performCalculations : "
                  "no more-greeks returned from pricing engine");
        /* no check on null values - just copy.
           this allows:
           a) to decide in derived options what to do when null
           results are returned (throw? numerical calculation?)
           b) to implement slim engines which only calculate the
           value---of course care must be taken not to call
           the greeks methods when using these.
        */
        strikeSensitivity_ = results->strikeSensitivity;

    }



    void OneAssetStrikedOption::arguments::validate() const {

        #if defined(QL_PATCH_MICROSOFT)
        OneAssetOption::arguments copy = *this;
        copy.validate();
        #else
        OneAssetOption::arguments::validate();
        #endif


    }


}

