
/*
 Copyright (C) 2003 Ferdinando Ametrano

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

/*! \file quantoforwardvanillaoption.cpp
    \brief Quanto version of a forward vanilla option
*/

#include <ql/Instruments/quantoforwardvanillaoption.hpp>
#include <ql/Instruments/forwardvanillaoption.hpp>

namespace QuantLib {

    namespace Instruments {

        QuantoForwardVanillaOption::QuantoForwardVanillaOption(
            Option::Type type,
            const RelinkableHandle<MarketElement>& underlying,
            double strike,
            const RelinkableHandle<TermStructure>& dividendTS,
            const RelinkableHandle<TermStructure>& riskFreeTS,
            const Exercise& exercise,
            const RelinkableHandle<BlackVolTermStructure>& volTS,
            const Handle<PricingEngine>& engine,
            const RelinkableHandle<TermStructure>& foreignRiskFreeTS,
            const RelinkableHandle<BlackVolTermStructure>& exchRateVolTS,
            const RelinkableHandle<MarketElement>& correlation,
            double moneyness,
            Date resetDate,
            const std::string& isinCode,
            const std::string& description)
        : QuantoVanillaOption(type, underlying, strike, dividendTS, riskFreeTS,
          exercise, volTS, engine, foreignRiskFreeTS, exchRateVolTS,
          correlation, isinCode, description), moneyness_(moneyness),
          resetDate_(resetDate) {
            QL_REQUIRE(!engine.isNull(),
                "QuantoForwardVanillaOption::QuantoForwardVanillaOption : "
                "null engine or wrong engine type");
        }

        void QuantoForwardVanillaOption::setupArguments(Arguments* args) 
                                                                    const {
            VanillaOption::setupArguments(args);
            QuantoForwardVanillaOption::arguments* arguments =
                dynamic_cast<QuantoForwardVanillaOption::arguments*>(args);
            QL_REQUIRE(arguments != 0,
               "QuantoForwardVanillaOption::setupArguments() : "
               "pricing engine does not supply needed arguments");

            arguments->foreignRiskFreeTS = foreignRiskFreeTS_;
            arguments->exchRateVolTS = exchRateVolTS_;
            QL_REQUIRE(!correlation_.isNull(),
                "QuantoVanillaOption::setupArguments() : "
                "null correlation given");
            arguments->correlation =
                correlation_->value();

            arguments->moneyness = moneyness_;
            arguments->resetDate = resetDate_;

        }

    }

}

