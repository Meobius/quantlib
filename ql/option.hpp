
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

/*! \file option.hpp
    \brief Base option class
*/

#ifndef quantlib_option_h
#define quantlib_option_h

#include <ql/instrument.hpp>
#include <ql/payoff.hpp>
#include <ql/exercise.hpp>

namespace QuantLib {

    //! base option class
    class Option : public Instrument {
      public:
        class arguments;
        enum Type { Call, Put, Straddle };
        Option(const Handle<Payoff>& payoff,
               const Handle<Exercise>& exercise,
               const Handle<PricingEngine>& engine = Handle<PricingEngine>(),
               const std::string& isinCode = "",
               const std::string& description = "")
        : Instrument(isinCode, description), payoff_(payoff),
          exercise_(exercise) {
            if (!IsNull(engine))
                setPricingEngine(engine);
        }
      protected:
        // arguments
        Handle<Payoff> payoff_;
        Handle<Exercise> exercise_;
    };


    //! basic %option arguments
    class Option::arguments : public virtual Arguments {
      public:
        arguments() {}
        void validate() const {
            QL_REQUIRE(!IsNull(payoff),
                       "Option::arguments::validate() : "
                       "no payoff given");
        }
        Handle<Payoff> payoff;
        Handle<Exercise> exercise;
        // shouldn't be here
        // it should be moved elsewhere
        std::vector<Time> stoppingTimes;
    };

    //! additional %option results
    class Greeks : public virtual Results {
      public:
        Greeks() { reset(); }
        void reset() {
            delta =  gamma = theta = vega =
                rho = dividendRho = Null<double>();
        }
        double delta, gamma;
        double theta;
        double vega;
        double rho, dividendRho;
    };

    //! more additional %option results
    // add here vomma, ect.
    class MoreGreeks : public virtual Results {
      public:
        MoreGreeks() { reset(); }
        void reset() {
            itmProbability = deltaForward = elasticity = thetaPerDay =
                strikeSensitivity = Null<double>();
        }
        double itmProbability, deltaForward, elasticity, thetaPerDay,
            strikeSensitivity;
    };

}


#endif
