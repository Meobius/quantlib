
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

/*! \file warsaw.hpp
    \brief Warsaw calendar
*/

#ifndef quantlib_warsaw_calendar_h
#define quantlib_warsaw_calendar_h

#include <ql/calendar.hpp>

namespace QuantLib {

    //! %Warsaw calendar
    /*! Holidays:
      <ul>
      <li>Saturdays</li>
      <li>Sundays</li>
      <li>Easter Monday</li>
      <li>Corpus Christi</li>
      <li>New Year's Day, January 1st</li>
      <li>May Day, May 1st</li>
      <li>Constitution Day, May 3rd</li>
      <li>Assumption of the Blessed Virgin Mary, August 15th</li>
      <li>All Saints Day, November 1st</li>
      <li>Independence Day, November 11th</li>
      <li>Christmas, December 25th</li>
      <li>2nd Day of Christmas, December 26th</li>
      </ul>
    */
    class Warsaw : public Calendar {
      private:
        class Impl : public Calendar::WesternImpl {
          public:
            std::string name() const { return "Warsaw"; }
            bool isBusinessDay(const Date&) const;
        };
      public:
        Warsaw()
        : Calendar(Handle<Calendar::Impl>(new Warsaw::Impl)) {}
    };

}


#endif
