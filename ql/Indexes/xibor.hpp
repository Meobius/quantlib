
/*
 Copyright (C) 2000, 2001, 2002 RiskMap srl

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

/*! \file xibor.hpp
    \brief base class for libor indexes

    \fullpath
    ql/Indexes/%xibor.hpp
*/

// $Id$

#ifndef quantlib_xibor_hpp
#define quantlib_xibor_hpp

#include <ql/index.hpp>
#include <ql/termstructure.hpp>

namespace QuantLib {

    namespace Indexes {

        //! base class for libor indexes
        class Xibor : public Index,
                      public Patterns::Observer {
          public:
            Xibor(const std::string& familyName,
                int n, TimeUnit units, int settlementDays,
                Currency currency,
                const Calendar& calendar, bool isAdjusted,
                RollingConvention rollingConvention,
                const DayCounter& dayCounter, 
                const RelinkableHandle<TermStructure>& h)
            : familyName_(familyName), n_(n), units_(units),
              settlementDays_(settlementDays),
              currency_(currency), calendar_(calendar),
              isAdjusted_(isAdjusted),
              rollingConvention_(rollingConvention),
              dayCounter_(dayCounter), termStructure_(h) {
                registerWith(termStructure_);
            }
            //! \name Index interface
            //@{
            Rate fixing(const Date& fixingDate) const;
            //@}
            //! \name Observer interface
            //@{
            void update();
            //@}
            //! \name Inspectors
            //@{
            std::string name() const;
            Period tenor() const;
            int frequency() const;
            int settlementDays() const;
            Currency currency() const;
            Calendar calendar() const;
            bool isAdjusted() const;
            RollingConvention rollingConvention() const;
            DayCounter dayCounter() const;
            Handle<TermStructure> termStructure() const;
            //@}
          private:
            std::string familyName_;
            int n_;
            TimeUnit units_;
            int settlementDays_;
            Currency currency_;
            Calendar calendar_;
            bool isAdjusted_;
            RollingConvention rollingConvention_;
            DayCounter dayCounter_;
            RelinkableHandle<TermStructure> termStructure_;
        };


        // inline definitions

        inline void Xibor::update() {
            notifyObservers();
        }
        
        inline Period Xibor::tenor() const { 
            return Period(n_,units_); 
        }
        
        inline int Xibor::settlementDays() const { 
            return settlementDays_; 
        }
        
        inline Currency Xibor::currency() const { 
            return currency_; 
        }
        
        inline Calendar Xibor::calendar() const { 
            return calendar_; 
        }
        
        inline bool Xibor::isAdjusted() const { 
            return isAdjusted_; 
        }
        
        inline RollingConvention Xibor::rollingConvention() const {
            return rollingConvention_; 
        }
        
        inline DayCounter Xibor::dayCounter() const { 
            return dayCounter_; 
        }
        
        inline Handle<TermStructure> Xibor::termStructure() const {
            return (*termStructure_).currentLink();
        }
        
    }

}


#endif
