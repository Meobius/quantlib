
/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file swaption.hpp
    \brief Swaption class
*/

#ifndef quantlib_instruments_swaption_h
#define quantlib_instruments_swaption_h

#include <ql/exercise.hpp>
#include <ql/numericalmethod.hpp>
#include <ql/Instruments/simpleswap.hpp>

namespace QuantLib {

    namespace Instruments {

        //! Swaption class
        class Swaption : public Option {
          public:
            class arguments;
            class results;
            Swaption(const Handle<SimpleSwap>& swap,
                     const Exercise& exercise,
                     const RelinkableHandle<TermStructure>& termStructure,
                     const Handle<PricingEngine>& engine);
            bool isExpired() const;
            void setupArguments(Arguments*) const;
          protected:
            void performCalculations() const;
          private:
            // arguments
            Handle<SimpleSwap> swap_;
            Exercise exercise_;
            const RelinkableHandle<TermStructure>& termStructure_;
        };

        //! arguments for swaption calculation
        class Swaption::arguments : public SimpleSwap::arguments {
          public:
            arguments() : fairRate(Null<double>()),
                          fixedRate(Null<double>()),
                          fixedBPS(Null<double>()),
                          exerciseType(Exercise::Type(-1)) {}
            Rate fairRate;
            Rate fixedRate;
            double fixedBPS;
            Exercise::Type exerciseType;
            std::vector<Time> exerciseTimes;
            void validate() const;
        };

        //! %results from swaption calculation
        class Swaption::results : public Value {};

    }

}


#endif
