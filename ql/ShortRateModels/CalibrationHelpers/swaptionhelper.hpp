
/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file swaptionhelper.hpp
    \brief Swaption calibration helper
*/

#ifndef quantlib_interest_rate_modelling_calibration_helpers_swaption_h
#define quantlib_interest_rate_modelling_calibration_helpers_swaption_h

#include <ql/ShortRateModels/calibrationhelper.hpp>
#include <ql/Instruments/swaption.hpp>


namespace QuantLib {

    class SwaptionHelper : public CalibrationHelper {
      public:
        //!Constructor for ATM swaption
        SwaptionHelper(const Period& maturity,
                       const Period& length,
                       const RelinkableHandle<Quote>& volatility,
                       const Handle<Xibor>& index,
                       const RelinkableHandle<TermStructure>& termStructure);

        virtual void addTimesTo(std::list<Time>& times) const;

        virtual double modelValue();

        virtual double blackPrice(double volatility) const;

      private:
        Rate exerciseRate_;
        Handle<SimpleSwap> swap_;
        Handle<Swaption> swaption_;
    };

}


#endif
