
/*
 Copyright (C) 2000-2003 RiskMap srl

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

/*! \file forwardspreadedtermstructure.hpp
    \brief Forward spreaded term structure
*/

#ifndef quantlib_forward_spreaded_term_structure_hpp
#define quantlib_forward_spreaded_term_structure_hpp

#include <ql/termstructure.hpp>

namespace QuantLib {
    namespace TermStructures {

        //! Term structure with added spread on the instantaneous forward rate
        /*! \note This term structure will remain linked to the original
                structure, i.e., any changes in the latter will be reflected in
                this structure as well.
        */
        class ForwardSpreadedTermStructure : public ForwardRateStructure,
                                             public Patterns::Observer {
          public:
            ForwardSpreadedTermStructure(
                const RelinkableHandle<TermStructure>&,
                const RelinkableHandle<MarketElement>& spread);
            //! \name TermStructure interface
            //@{
            DayCounter dayCounter() const;
            Date todaysDate() const;
            Date referenceDate() const;
            Date maxDate() const;
            Time maxTime() const;
            //@}
            //! \name Observer interface
            //@{
            void update();
            //@}
          protected:
            //! returns the spreaded forward rate
            Rate forwardImpl(Time, bool extrapolate = false) const;
            //! returns the spreaded zero yield rate
            /*! \warning This method must disappear should the spread become a
                         curve
            */
            Rate zeroYieldImpl(Time, bool extrapolate = false) const;
          private:
            RelinkableHandle<TermStructure> originalCurve_;
            RelinkableHandle<MarketElement> spread_;
        };

        inline ForwardSpreadedTermStructure::ForwardSpreadedTermStructure(
            const RelinkableHandle<TermStructure>& h,
            const RelinkableHandle<MarketElement>& spread)
        : originalCurve_(h), spread_(spread) {
            registerWith(originalCurve_);
            registerWith(spread_);
        }

        inline DayCounter ForwardSpreadedTermStructure::dayCounter() const {
            return originalCurve_->dayCounter();
        }

        inline Date ForwardSpreadedTermStructure::todaysDate() const {
            return originalCurve_->todaysDate();
        }

        inline Date ForwardSpreadedTermStructure::referenceDate() const {
            return originalCurve_->referenceDate();
        }

        inline Date ForwardSpreadedTermStructure::maxDate() const {
            return originalCurve_->maxDate();
        }

        inline Time ForwardSpreadedTermStructure::maxTime() const {
            return originalCurve_->maxTime();
        }

        inline void ForwardSpreadedTermStructure::update() {
            notifyObservers();
        }

        inline Rate ForwardSpreadedTermStructure::forwardImpl(Time t,
            bool extrapolate) const {
                return originalCurve_->instantaneousForward(t, extrapolate) +
                    spread_->value();
        }

        inline Rate ForwardSpreadedTermStructure::zeroYieldImpl(Time t,
            bool extrapolate) const {
                return originalCurve_->zeroYield(t, extrapolate) +
                    spread_->value();
        }

    }

}


#endif
