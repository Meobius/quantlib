
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

/*! \file instrument.hpp
    \brief Abstract instrument class
*/

#ifndef quantlib_instrument_h
#define quantlib_instrument_h

#include <ql/Patterns/lazyobject.hpp>
#include <ql/pricingengine.hpp>

namespace QuantLib {

    //! Abstract instrument class
    /*! This class is purely abstract and defines the interface of concrete
        instruments which will be derived from this one.
    */
    class Instrument : public LazyObject {
      public:
        Instrument(const std::string& isinCode = "",
                   const std::string& description = "");
        //! \name Inspectors
        //@{
        //! returns the ISIN code of the instrument, when given.
        std::string isinCode() const;
        //! returns a brief textual description of the instrument.
        std::string description() const;
        //! returns the net present value of the instrument.
        double NPV() const;
        //! returns the error estimate on the NPV when available.
        double errorEstimate() const;
        //! returns whether the instrument is still tradable.
        virtual bool isExpired() const = 0;
        //@}
        //! \name Modifiers
        //@{
        //! set the pricing engine to be used.
        /*! \warning calling this method will have no effects in
                     case the <b>performCalculation</b> method
                     was overridden in a derived class.
        */
        void setPricingEngine(const Handle<PricingEngine>&);
        //@}
        /*! When a derived argument structure is defined for an instrument,
            this method should be overridden to fill it. This is mandatory
            in case a pricing engine is used.
        */
        virtual void setupArguments(Arguments*) const;
      protected:
        //! \name Calculations 
        //@{
        void calculate() const;
        /*! This method must leave the instrument in a consistent
            state when the expiration condition is met.
        */
        virtual void setupExpired() const;
        /*! In case a pricing engine is <b>not</b> used, this
            method must be overridden to perform the actual 
            calculations and set any needed results. In case
            a pricing engine is used, the default implementation
            can be used.
        */
        virtual void performCalculations() const;
        //@}
        /*! \name Results
            The value of this attribute and any other that derived 
            classes might declare must be set during calculation.
        */
        //@{
        mutable double NPV_, errorEstimate_;
        //@}
      protected:
        Handle<PricingEngine> engine_;
      private:
        std::string isinCode_, description_;
    };

    //! pricing results
    class Value : public virtual Results {
      public:
        Value() { reset(); }
        void reset() {
            value = errorEstimate = Null<double>();
        }
        double value;
        double errorEstimate;
    };


    // inline definitions

    inline Instrument::Instrument(const std::string& isinCode,
                                  const std::string& description)
    : NPV_(0.0), errorEstimate_(Null<double>()), 
      isinCode_(isinCode), description_(description) {}

    inline std::string Instrument::isinCode() const {
        return isinCode_;
    }

    inline std::string Instrument::description() const {
        return description_;
    }

    inline void Instrument::setPricingEngine(const Handle<PricingEngine>& e) {
        QL_REQUIRE(!e.isNull(), "Instrument: null pricing engine");
        try {
            unregisterWith(engine_);
        } catch (...) {
            // null or not observable---never mind
        }
        engine_ = e;
        try {
            registerWith(engine_);
        } catch (...) {
            // not observable---never mind
        }
        // trigger (lazy) recalculation and notify observers
        update();
    }

    inline void Instrument::setupArguments(Arguments*) const {
        throw Error("setupArguments() not implemented");
    }

    inline void Instrument::calculate() const {
        if (isExpired()) {
            setupExpired();
            calculated_ = true;
        } else {
            LazyObject::calculate();
        }
    }

    inline void Instrument::setupExpired() const {
        NPV_ = errorEstimate_ = 0.0;
    }

    inline void Instrument::performCalculations() const {
        QL_REQUIRE(!engine_.isNull(), "Instrument: null pricing engine");
        engine_->reset();
        setupArguments(engine_->arguments());
        engine_->arguments()->validate();
        engine_->calculate();
        const Value* results = dynamic_cast<const Value*>(engine_->results());
        QL_ENSURE(results != 0,
                  "Instrument: no results returned from pricing engine");
        NPV_ = results->value;
        errorEstimate_ = results->errorEstimate;
    }

    inline double Instrument::NPV() const {
        calculate();
        return NPV_;
    }

    inline double Instrument::errorEstimate() const {
        calculate();
        QL_REQUIRE(errorEstimate_ != Null<double>(),
                   "error estimate not provided");
        return errorEstimate_;
    }

}


#endif
