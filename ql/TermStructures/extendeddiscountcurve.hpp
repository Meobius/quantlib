
/*
  Copyright (C) 2002, 2003 Decillion Pty(Ltd).

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

/*! \file extendeddiscountcurve.hpp
    \brief discount factor structure with detailed compound-forward calculation
*/

#ifndef quantlib_extended_discount_curve_h
#define quantlib_extended_discount_curve_h

#include <ql/TermStructures/discountcurve.hpp>
#include <map>

namespace QuantLib {

    namespace TermStructures {

        //! Term structure based on loglinear interpolation of discount factors
        /*! Loglinear interpolation guarantees piecewise constant forward
            rates.

            Rates are assumed to be annual continuos compounding.
        */
        class ExtendedDiscountCurve : public DiscountCurve,
                                      public Patterns::Observer {
          public:
            // constructor
            ExtendedDiscountCurve(const Date &todaysDate,
                                  const std::vector<Date> &dates,
                                  const std::vector<DiscountFactor> &dfs,
                                  const Calendar & calendar,
                                  const RollingConvention roll,
                                  const DayCounter & dayCounter = Actual365());
            Calendar calendar() const { return calendar_; };
            RollingConvention roll() const { return roll_; };
            //! \name Observer interface
            //@{
            void update();
            //@}
          protected:
            void calibrateNodes() const;
            Handle<TermStructure> reversebootstrap(int) const;
            Rate compoundForwardImpl(Time, int, 
                                     bool extrapolate = false) const;
            Handle<TermStructure> forwardCurve(int) const;
          private:
            Calendar calendar_;
            RollingConvention roll_;
            mutable std::map<int,Handle<TermStructure> > forwardCurveMap_;
        };

        // inline definitions

        inline void ExtendedDiscountCurve::update() {
            forwardCurveMap_.clear();
            notifyObservers();
        }

    }

}


#endif
