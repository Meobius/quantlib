
/*
 Copyright (C) 2003 RiskMap srl

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
// $Id$

#include "dates.hpp"

using namespace QuantLib;

void DateTest::runTest() {
    
    int minDate = Date::minDate().serialNumber()+1,
        maxDate = Date::maxDate().serialNumber();

    int dyold = Date(minDate-1).dayOfYear(),
        dold  = Date(minDate-1).dayOfMonth(),
        mold  = Date(minDate-1).month(),
        yold  = Date(minDate-1).year(),
        wdold = Date(minDate-1).weekday();

    for (int i=minDate; i<=maxDate; i++) {
        Date t(i);
        int serial = t.serialNumber();

        // check serial number consistency
        if (serial != i)
            CPPUNIT_FAIL(
                "inconsistent serial number:\n"
                "    original:      " + IntegerFormatter::toString(i) + "\n"
                "    date:          " + DateFormatter::toString(t) + "\n"
                "    serial number: " + IntegerFormatter::toString(serial));
        
        int dy = t.dayOfYear(),
            d  = t.dayOfMonth(),
            m  = t.month(),
            y  = t.year(),
            wd = t.weekday();

        // check if skipping any date
        if (!((dy == dyold+1) ||
              (dy == 1 && dyold == 365 && !Date::isLeap(yold)) ||
              (dy == 1 && dyold == 366 && Date::isLeap(yold))))
            CPPUNIT_FAIL(
                "wrong day of year increment: \n"
                "    date: " + DateFormatter::toString(t) + "\n"
                "    day of year: " + IntegerFormatter::toString(dy) + "\n"
                "    previous:    " + IntegerFormatter::toString(dyold));
        dyold = dy;

        if (!((d == dold+1 && m == mold   && y == yold) ||
              (d == 1      && m == mold+1 && y == yold) ||
              (d == 1      && m == 1      && y == yold+1)))
            CPPUNIT_FAIL(
                "wrong day,month,year increment: \n"
                "    date: " + DateFormatter::toString(t) + "\n"
                "    day,month,year: " + IntegerFormatter::toString(d) + ","
                                       + IntegerFormatter::toString(m) + ","
                                       + IntegerFormatter::toString(y) + "\n"
                "    previous:       " + IntegerFormatter::toString(dold) + ","
                                       + IntegerFormatter::toString(mold) + ","
                                       + IntegerFormatter::toString(yold));
        dold = d; mold = m; yold = y;
        
        // check month definition
        if (m < 1 || m > 12)
            CPPUNIT_FAIL(
                "invalid month: \n"
                "    date:  " + DateFormatter::toString(t) + "\n"
                "    month: " + IntegerFormatter::toString(m));

        // check day definition
        if (d < 1)
            CPPUNIT_FAIL(
                "invalid day of month: \n"
                "    date:  " + DateFormatter::toString(t) + "\n"
                "    day: " + IntegerFormatter::toString(d));
        if (!((m == 1  && d <= 31) ||
              (m == 2  && d <= 28) ||
              (m == 2  && d == 29 && Date::isLeap(y)) ||
              (m == 3  && d <= 31) ||
              (m == 4  && d <= 30) ||
              (m == 5  && d <= 31) ||
              (m == 6  && d <= 30) ||
              (m == 7  && d <= 31) ||
              (m == 8  && d <= 31) ||
              (m == 9  && d <= 30) ||
              (m == 10 && d <= 31) ||
              (m == 11 && d <= 30) ||
              (m == 12 && d <= 31)))
            CPPUNIT_FAIL(
                "invalid day of month: \n"
                "    date:  " + DateFormatter::toString(t) + "\n"
                "    day: " + IntegerFormatter::toString(d));

        // check weekday definition
        if (!((int(wd) == int(wdold+1)) ||
              (int(wd) == 1 && int(wdold) == 7)))
            CPPUNIT_FAIL(
                "invalid weekday: \n"
                "    date:  " + DateFormatter::toString(t) + "\n"
                "    weekday:  " + IntegerFormatter::toString(wd) + "\n"
                "    previous: " + IntegerFormatter::toString(wdold));
        wdold = wd;

        // create the same date with a different constructor
        Date s(d,Month(m),y);
        // check serial number consistency
        serial = s.serialNumber();
        if (serial != i)
            CPPUNIT_FAIL(
                "inconsistent serial number:\n"
                "    date:          " + DateFormatter::toString(t) + "\n"
                "    serial number: " + IntegerFormatter::toString(i) + "\n"
                "    cloned date:   " + DateFormatter::toString(s) + "\n"
                "    serial number: " + IntegerFormatter::toString(serial));
    }

}

