
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

/*! \file simpleswap.hpp
    \brief Simple fixed-rate vs Libor swap
*/

#ifndef quantlib_simple_swap_h
#define quantlib_simple_swap_h

#include <ql/Instruments/swap.hpp>
#include <ql/Indexes/xibor.hpp>

namespace QuantLib {

    namespace Instruments {

        //! Simple fixed-rate vs Libor swap
        class SimpleSwap : public Swap {
          public:
            SimpleSwap(bool payFixedRate,
                // dates
                const Date& startDate, int n, TimeUnit units,
                const Calendar& calendar,
                RollingConvention rollingConvention,
                double nominal,
                // fixed leg
                int fixedFrequency,
                Rate fixedRate,
                bool fixedIsAdjusted,
                const DayCounter& fixedDayCount,
                // floating leg
                int floatingFrequency,
                const Handle<Indexes::Xibor>& index,
                int indexFixingDays,
                Spread spread,
                // hook to term structure
                const RelinkableHandle<TermStructure>& termStructure,
                // description
                const std::string& isinCode = "",
                const std::string& description = "");
            // results
            Rate fairRate() const;
            Spread fairSpread() const;
            double fixedLegBPS() const;
            double floatingLegBPS() const;
            // inspectors
            Rate fixedRate() const;
            Spread spread() const;
            double nominal() const;
            const Date& maturity() const;
            bool payFixedRate() const;
            const std::vector<Handle<CashFlow> >& fixedLeg() const;
            const std::vector<Handle<CashFlow> >& floatingLeg() const;
          private:
            bool payFixedRate_;
            Rate fixedRate_;
            Spread spread_;
            double nominal_;
            Date maturity_;
        };

        inline Rate SimpleSwap::fairRate() const {
            return fixedRate_ - NPV()/fixedLegBPS();
        }

        inline Spread SimpleSwap::fairSpread() const {
            return spread_ - NPV()/floatingLegBPS();
        }

        inline double SimpleSwap::fixedLegBPS() const {
            return (payFixedRate_ ? firstLegBPS() : secondLegBPS());
        }

        inline double SimpleSwap::floatingLegBPS() const {
            return (payFixedRate_ ? secondLegBPS() : firstLegBPS());
        }

        inline Rate SimpleSwap::fixedRate() const {
            return fixedRate_;
        }

        inline Spread SimpleSwap::spread() const {
            return spread_;
        }

        inline double SimpleSwap::nominal() const {
            return nominal_;
        }

        inline const Date& SimpleSwap::maturity() const {
            return maturity_;
        }

        inline bool SimpleSwap::payFixedRate() const {
            return payFixedRate_;
        }

        inline const std::vector<Handle<CashFlow> >&
        SimpleSwap::fixedLeg() const {
            return (payFixedRate_ ? firstLeg_ : secondLeg_);
        }

        inline const std::vector<Handle<CashFlow> >&
        SimpleSwap::floatingLeg() const {
            return (payFixedRate_ ? secondLeg_ : firstLeg_);
        }

    }

}


#endif
