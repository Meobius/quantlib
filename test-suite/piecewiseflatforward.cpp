
/*
 Copyright (C) 2003 RiskMap srl

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

#include "piecewiseflatforward.hpp"
#include "utilities.hpp"
#include <ql/TermStructures/piecewiseflatforward.hpp>
#include <ql/Calendars/target.hpp>
#include <ql/DayCounters/actual360.hpp>
#include <ql/DayCounters/thirty360.hpp>
#include <ql/Indexes/euribor.hpp>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

using namespace QuantLib;

#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

namespace {

    struct Datum {
        int n;
        TimeUnit units;
        double rate;
    };

    Datum depositData[] = {
        { 1, Weeks,  4.559 },
        { 1, Months, 4.581 },
        { 2, Months, 4.573 },
        { 3, Months, 4.557 },
        { 6, Months, 4.496 },
        { 9, Months, 4.490 }
    };
    
    Datum swapData[] = {
        {  1, Years, 4.54 },
        {  2, Years, 4.63 },
        {  3, Years, 4.75 },
        {  4, Years, 4.86 },
        {  5, Years, 4.99 },
        {  6, Years, 5.11 },
        {  7, Years, 5.23 },
        {  8, Years, 5.33 },
        {  9, Years, 5.41 },
        { 10, Years, 5.47 },
        { 12, Years, 5.60 },
        { 15, Years, 5.75 },
        { 20, Years, 5.89 },
        { 25, Years, 5.95 },
        { 30, Years, 5.96 }
    };
    
    // test-global variables

    Calendar calendar;
    int settlementDays, fixingDays;
    Date today, settlement;
    RollingConvention depoRollingConvention;
    DayCounter depoDayCounter;
    RollingConvention swapRollingConvention;
    int fixedLegFrequency;
    bool fixedLegIsAdjusted;
    DayCounter fixedLegDayCounter;
    int floatingLegFrequency;

    Size deposits, swaps;
    std::vector<Handle<SimpleMarketElement> > rates;
    Handle<TermStructure> termStructure;
}

void PiecewiseFlatForwardTest::setUp() {

    // data
    calendar = TARGET();
    settlementDays = 2;
    fixingDays = 2;
    today = calendar.roll(Date::todaysDate());
    settlement = calendar.advance(today,settlementDays,Days);
    depoRollingConvention = ModifiedFollowing;
    depoDayCounter = Actual360();
    swapRollingConvention = ModifiedFollowing;
    fixedLegFrequency = 1;
    fixedLegIsAdjusted = false;
    fixedLegDayCounter = Thirty360();
    floatingLegFrequency = 2;

    deposits = LENGTH(depositData);
    swaps = LENGTH(swapData);

    // market elements
    rates = std::vector<Handle<SimpleMarketElement> >(deposits+swaps);
    Size i;
    for (i=0; i<deposits; i++) {
        rates[i] = Handle<SimpleMarketElement>(
            new SimpleMarketElement(depositData[i].rate/100));
    }
    for (i=0; i<swaps; i++) {
        rates[i+deposits] = Handle<SimpleMarketElement>(
            new SimpleMarketElement(swapData[i].rate/100));
    }

    // rate helpers
    std::vector<Handle<RateHelper> > instruments(deposits+swaps);
    for (i=0; i<deposits; i++) {
        RelinkableHandle<MarketElement> r(rates[i]);
        instruments[i] = Handle<RateHelper>(
            new DepositRateHelper(r, depositData[i].n, depositData[i].units,
                                  settlementDays, calendar,
                                  depoRollingConvention, depoDayCounter));
    }
    for (i=0; i<swaps; i++) {
        RelinkableHandle<MarketElement> r(rates[i+deposits]);
        instruments[i+deposits] = Handle<RateHelper>(
            new SwapRateHelper(r, swapData[i].n, swapData[i].units,
                               settlementDays, calendar,
                               swapRollingConvention,
                               fixedLegFrequency, fixedLegIsAdjusted,
                               fixedLegDayCounter, floatingLegFrequency));
    }
    
    // instantiate curve
    termStructure = Handle<TermStructure>(
        new PiecewiseFlatForward(today,settlement,instruments,Actual360()));
    
}

void PiecewiseFlatForwardTest::testConsistency() {

    RelinkableHandle<TermStructure> euriborHandle;
    euriborHandle.linkTo(termStructure);

    Size i;
    // check deposits
    for (i=0; i<deposits; i++) {
        Euribor index(depositData[i].n,depositData[i].units,euriborHandle);
        double expectedRate  = depositData[i].rate/100,
               estimatedRate = index.fixing(today);
        if (QL_FABS(expectedRate-estimatedRate) > 1.0e-9) {
            CPPUNIT_FAIL(
                IntegerFormatter::toString(depositData[i].n) + " "
                + (depositData[i].units == Weeks ? std::string("week(s)") :
                                                   std::string("month(s)")) +
                " deposit: \n"
                "    estimated rate: "
                + RateFormatter::toString(estimatedRate,8) + "\n"
                "    expected rate:  "
                + RateFormatter::toString(expectedRate,8));
        }
    }

    // check swaps
    Handle<Xibor> index(new Euribor(12/floatingLegFrequency,Months,
                                    euriborHandle));
    for (i=0; i<swaps; i++) {
        SimpleSwap swap(true,settlement,swapData[i].n,swapData[i].units,
                        calendar,swapRollingConvention,100.0,
                        fixedLegFrequency,0.0,fixedLegIsAdjusted,
                        fixedLegDayCounter,floatingLegFrequency,index,
                        fixingDays,0.0,euriborHandle);
        double expectedRate = swapData[i].rate/100,
               estimatedRate = swap.fairRate();
        if (QL_FABS(expectedRate-estimatedRate) > 1.0e-9) {
            CPPUNIT_FAIL(
                IntegerFormatter::toString(swapData[i].n) + " year(s) swap:\n"
                "    estimated rate: "
                + RateFormatter::toString(estimatedRate,8) + "\n"
                "    expected rate:  "
                + RateFormatter::toString(expectedRate,8));
        }
    }
}

void PiecewiseFlatForwardTest::testObservability() {

    Flag f;
    f.registerWith(termStructure);

    for (Size i=0; i<deposits+swaps; i++) {
        f.lower();
        rates[i]->setValue(rates[i]->value()*1.01);
        if (!f.isUp())
            CPPUNIT_FAIL("Observer was not notified "
                         "of piecewise flat forward curve change");
    }
}


CppUnit::Test* PiecewiseFlatForwardTest::suite() {
    CppUnit::TestSuite* tests = 
        new CppUnit::TestSuite("Piecewise flat forward tests");
    tests->addTest(new CppUnit::TestCaller<PiecewiseFlatForwardTest>
                   ("Testing consistency of piecewise flat forward curve",
                    &PiecewiseFlatForwardTest::testConsistency));
    tests->addTest(new CppUnit::TestCaller<PiecewiseFlatForwardTest>
                   ("Testing observability of piecewise flat forward curve",
                    &PiecewiseFlatForwardTest::testObservability));
    return tests;
}

