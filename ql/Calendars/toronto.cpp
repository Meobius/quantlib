

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
/*! \file toronto.cpp
    \brief Toronto calendar

    \fullpath
    Calendars/%toronto.cpp
*/

// $Id$

#include <ql/Calendars/toronto.hpp>

namespace QuantLib {

    namespace Calendars {

        bool Toronto::TorCalendarImpl::isBusinessDay(const Date& date) const {
            Weekday w = date.weekday();
            Day d = date.dayOfMonth(), dd = date.dayOfYear();
            Month m = date.month();
            Year y = date.year();
            Day em = easterMonday(y);
            if ((w == Saturday || w == Sunday)
                // New Year's Day (possibly moved to Monday)
                || ((d == 1 || (d == 2 && w == Monday)) && m == January)
                // Good Friday
                || (dd == em-3)
                // Easter Monday
                || (dd == em)
                // The Monday on or preceding 24 May (Victoria Day)
                || (d > 17 && d <= 24 && w == Monday && m == May)
                // July 1st, possibly moved to Monday (Canada Day)
                || ((d == 1 || ((d == 2 || d == 3) && w == Monday)) &&  m == July)
                // first Monday of August (Provincial Holiday)
                || (d <= 7 && w == Monday && m == August)
                // first Monday of September (Labor Day)
                || (d <= 7 && w == Monday && m == September)
                // second Monday of October (Thanksgiving Day)
                || (d > 7 && d <= 14 && w == Monday && m == October)
                // November 11th (Remembrance Day)
                || ((d == 11 || (d == 12 && w == Monday)) && m == November)
                // Christmas (possibly moved to Monday or Tuesday)
                || ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
                    && m == December)
                // Boxing Day (possibly moved to Monday or Tuesday)
                || ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
                    && m == December)
                    )
                    return false;
            return true;
        }

    }

}

