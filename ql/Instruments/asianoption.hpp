
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

/*! \file asianoption.hpp
    \brief Asian option on a single asset
*/

#ifndef quantlib_asian_option_h
#define quantlib_asian_option_h

#include <ql/Instruments/oneassetstrikedoption.hpp>
#include <vector>

namespace QuantLib {

    //! placeholder for enumerated averaging types
    struct Average {
        enum Type { Arithmetic, Geometric };
    };

    //! Asian option
    class DiscreteAveragingAsianOption : public OneAssetStrikedOption {
      public:
          DiscreteAveragingAsianOption(
                    Average::Type averageType,
                    Handle<StrikedTypePayoff> payoff,
                    const RelinkableHandle<Quote>& underlying,
                    double runningProduct,
                    Size pastFixings,
                    std::vector<Date> fixingDates,
                    const RelinkableHandle<TermStructure>& dividendTS,
                    const RelinkableHandle<TermStructure>& riskFreeTS,
                    const Exercise& exercise,
                    const RelinkableHandle<BlackVolTermStructure>& volTS,
                    const Handle<PricingEngine>& engine =
                    Handle<PricingEngine>(),
                    const std::string& isinCode = "",
                    const std::string& description = "");
        //! \name Instrument interface
        //@{
        class arguments;
        //@}
        void setupArguments(Arguments*) const;
      protected:
        // enforce in this class any check on engine/payoff
        void performCalculations() const;
        // arguments
        Average::Type averageType_;
        double runningProduct_;
        Size pastFixings_;
        std::vector<Date> fixingDates_;
    };

    //! extra arguments for single asset asian option calculation
    class DiscreteAveragingAsianOption::arguments : public OneAssetStrikedOption::arguments {
      public:
        void validate() const;
        Average::Type averageType;
        double runningProduct;
        Size pastFixings;
        std::vector<Date> fixingDates;
    };



}


#endif

