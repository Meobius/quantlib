
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

/*! \file capfloor.hpp
    \brief Cap and Floor class
*/

#ifndef quantlib_instruments_capfloor_h
#define quantlib_instruments_capfloor_h

#include <ql/numericalmethod.hpp>
#include <ql/instrument.hpp>
#include <ql/cashflow.hpp>
#include <ql/termstructure.hpp>

namespace QuantLib {

    namespace Instruments {

        //! Base class for cap-like instruments
        class CapFloor : public Instrument {
          public:
            enum Type { Cap, Floor, Collar };
            class arguments;
            class results;
            CapFloor(Type type,
                     const std::vector<Handle<CashFlow> >& floatingLeg,
                     const std::vector<Rate>& capRates,
                     const std::vector<Rate>& floorRates,
                     const RelinkableHandle<TermStructure>& termStructure,
                     const Handle<PricingEngine>& engine);
            //! \name Instrument interface
            //@{
            bool isExpired() const;
            //@}
            //! \name Inspectors
            //@{
            Type type() const { return type_; }
            const std::vector<Handle<CashFlow> >& leg() const {
                return floatingLeg_;
            }
            const std::vector<Rate>& capRates() const {
                return capRates_;
            }
            const std::vector<Rate>& floorRates() const {
                return floorRates_;
            }
            //@}
            void setupArguments(Arguments*) const;
            //! implied term volatility
            double impliedVolatility(double price,
                                     double accuracy = 1.0e-4,
                                     Size maxEvaluations = 100,
                                     double minVol = 1.0e-4,
                                     double maxVol = 4.0) const;
          private:
            Type type_;
            std::vector<Handle<CashFlow> > floatingLeg_;
            std::vector<Rate> capRates_;
            std::vector<Rate> floorRates_;
            RelinkableHandle<TermStructure> termStructure_;
            // helper class for implied volatility calculation
            class ImpliedVolHelper {
              public:
                ImpliedVolHelper(const CapFloor&,
                                 const RelinkableHandle<TermStructure>&,
                                 double targetValue);
                double operator()(double x) const;
              private:
                Handle<PricingEngine> engine_;
                RelinkableHandle<TermStructure> termStructure_;
                double targetValue_;
                Handle<SimpleMarketElement> vol_;
                const Value* results_;
            };
        };

        //! Concrete cap class
        class Cap : public CapFloor {
          public:
            Cap(const std::vector<Handle<CashFlow> >& floatingLeg,
                const std::vector<Rate>& exerciseRates,
                const RelinkableHandle<TermStructure>& termStructure,
                const Handle<PricingEngine>& engine)
            : CapFloor(CapFloor::Cap, floatingLeg, 
                       exerciseRates, std::vector<Rate>(),
                       termStructure, engine) {}
        };

        //! Concrete floor class
        class Floor : public CapFloor {
          public:
            Floor(const std::vector<Handle<CashFlow> >& floatingLeg,
                  const std::vector<Rate>& exerciseRates,
                  const RelinkableHandle<TermStructure>& termStructure,
                  const Handle<PricingEngine>& engine)
            : CapFloor(CapFloor::Floor, floatingLeg, 
                       std::vector<Rate>(), exerciseRates,
                       termStructure, engine) {}
        };

        //! Concrete cap class
        class Collar : public CapFloor {
          public:
            Collar(const std::vector<Handle<CashFlow> >& floatingLeg,
                   const std::vector<Rate>& capRates,
                   const std::vector<Rate>& floorRates,
                   const RelinkableHandle<TermStructure>& termStructure,
                   const Handle<PricingEngine>& engine)
            : CapFloor(CapFloor::Collar, floatingLeg, capRates, floorRates,
                       termStructure, engine) {}
        };

        /*! \deprecated use CapFloor instead */
        typedef CapFloor VanillaCapFloor;

        /*! \deprecated use Cap instead */
        typedef Cap VanillaCap;

        /*! \deprecated use Floor instead */
        typedef Floor VanillaFloor;

        /*! \deprecated use Collar instead */
        typedef Collar VanillaCollar;


        //! arguments for cap/floor calculation
        class CapFloor::arguments : public virtual Arguments {
          public:
            arguments() : type(CapFloor::Type(-1)) {}
            CapFloor::Type type;
            std::vector<Time> startTimes;
            std::vector<Time> fixingTimes;
            std::vector<Time> endTimes;
            std::vector<Time> accrualTimes;
            std::vector<Rate> capRates;
            std::vector<Rate> floorRates;
            std::vector<Rate> forwards;
            std::vector<double> nominals;
            void validate() const;
        };

        //! %results from cap/floor calculation
        class CapFloor::results : public Value {};

    }

}


#endif
