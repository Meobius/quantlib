
/*
 Copyright (C) 2003 Ferdinando Ametrano
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

#include "europeanoption.hpp"
#include <ql/DayCounters/actual360.hpp>
#include <ql/Instruments/vanillaoption.hpp>
#include <ql/PricingEngines/Vanilla/mceuropeanengine.hpp>
#include <ql/TermStructures/flatforward.hpp>
#include <ql/Volatilities/blackconstantvol.hpp>

#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <map>

// This makes it easier to use array literals (alas, no std::vector literals)
#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

using namespace QuantLib;

namespace {

    // utilities

    Handle<TermStructure> makeFlatCurve(const Handle<Quote>& forward,
        DayCounter dc) {
        Date today = Date::todaysDate();
        return Handle<TermStructure>(new
            FlatForward(today, today, RelinkableHandle<Quote>(forward), dc));
    }

    Handle<BlackVolTermStructure> makeFlatVolatility(
        const Handle<Quote>& volatility, DayCounter dc) {
        Date today = Date::todaysDate();
        return Handle<BlackVolTermStructure>(new
            BlackConstantVol(today, RelinkableHandle<Quote>(volatility), dc));
    }

    enum EngineType { Analytic,
                      JR, CRR, EQP, TGEO, TIAN, LR,
                      PseudoMonteCarlo, QuasiMonteCarlo };

    double relativeError(double x1, double x2, double reference) {
        if (reference != 0.0)
            return QL_FABS(x1-x2)/reference;
        else
            return 1.0e+10;
    }

    Handle<VanillaOption>
    makeOption(const Handle<StrikedTypePayoff>& payoff,
               const Handle<Exercise>& exercise,
               const Handle<Quote>& u,
               const Handle<TermStructure>& q,
               const Handle<TermStructure>& r,
               const Handle<BlackVolTermStructure>& vol,
               EngineType engineType = Analytic) {

        Size binomialSteps = 251;
        Handle<PricingEngine> engine;
        switch (engineType) {
          case Analytic:
            engine = Handle<PricingEngine>(new AnalyticEuropeanEngine);
            break;
          case JR:
            engine = Handle<PricingEngine>(
                new BinomialVanillaEngine<JarrowRudd>(binomialSteps));
            break;
          case CRR:
            engine = Handle<PricingEngine>(
                new BinomialVanillaEngine<CoxRossRubinstein>(binomialSteps));
          case EQP:
            engine = Handle<PricingEngine>(
                new BinomialVanillaEngine<AdditiveEQPBinomialTree>(binomialSteps));
            break;
          case TGEO:
            engine = Handle<PricingEngine>(
                new BinomialVanillaEngine<Trigeorgis>(binomialSteps));
            break;
          case TIAN:
            engine = Handle<PricingEngine>(
                new BinomialVanillaEngine<Tian>(binomialSteps));
            break;
          case LR:
            engine = Handle<PricingEngine>(
                new BinomialVanillaEngine<LeisenReimer>(binomialSteps));
            break;
          case PseudoMonteCarlo:
            #if defined(QL_PATCH_MICROSOFT)
            engine = Handle<PricingEngine>(new
                MCEuropeanEngine<PseudoRandom>(1,
                                               false, false,
                                               Null<int>(), 0.05,
                                               Null<int>(), 42));
            #else
            engine = MakeMCEuropeanEngine<PseudoRandom>().withStepsPerYear(1)
                                                         .withTolerance(0.05)
                                                         .withSeed(42);
            #endif
            break;
          case QuasiMonteCarlo:
            #if defined(QL_PATCH_MICROSOFT)
            engine = Handle<PricingEngine>(new
                MCEuropeanEngine<LowDiscrepancy>(1,
                                                 false, false,
                                                 1023, Null<double>(),
                                                 Null<int>()));
            #else
            engine = MakeMCEuropeanEngine<LowDiscrepancy>().withStepsPerYear(1)
                                                           .withSamples(1023);
            #endif
            break;
          default:
            throw Error("Unknown engine type");
        }


        Handle<BlackScholesStochasticProcess> stochProcess(new
            BlackScholesStochasticProcess(
                RelinkableHandle<Quote>(u),
                RelinkableHandle<TermStructure>(q),
                RelinkableHandle<TermStructure>(r),
                RelinkableHandle<BlackVolTermStructure>(vol)));

        return Handle<VanillaOption>(new
            VanillaOption(stochProcess, payoff, exercise, engine));
    }

    std::string engineTypeToString(EngineType type) {
        switch (type) {
          case Analytic:
            return "analytic";
          case JR:
            return "Jarrow-Rudd";
          case CRR:
            return "Cox-Ross-Rubinstein";
          case EQP:
            return "EQP";
          case TGEO:
            return "Trigeorgis";
          case TIAN:
            return "Tian";
          case LR:
            return "LeisenReimer";
          case PseudoMonteCarlo:
            return "MonteCarlo";
          case QuasiMonteCarlo:
            return "Quasi-MonteCarlo";
          default:
            throw Error("unknown engine type");
        }
    }

    std::string payoffTypeToString(const Handle<Payoff>& payoff) {

        // PlainVanillaPayoff?
        Handle<PlainVanillaPayoff> pv;
        #if defined(HAVE_BOOST)
        pv = boost::dynamic_pointer_cast<PlainVanillaPayoff>(payoff);
        #else
        try {
            pv = payoff;
        } catch (...) {}
        #endif
        if (!IsNull(pv)) {
            // ok, the payoff is PlainVanillaPayoff
            return "PlainVanillaPayoff";
        }

        // CashOrNothingPayoff?
        Handle<CashOrNothingPayoff> coo;
        #if defined(HAVE_BOOST)
        coo = boost::dynamic_pointer_cast<CashOrNothingPayoff>(payoff);
        #else
        try {
            coo = payoff;
        } catch (...) {}
        #endif
        if (!IsNull(coo)) {
            // ok, the payoff is CashOrNothingPayoff
            return "Cash ("
                + DoubleFormatter::toString(coo->cashPayoff())
                + ") or Nothing Payoff";
        }

        // AssetOrNothingPayoff?
        Handle<AssetOrNothingPayoff> aoo;
        #if defined(HAVE_BOOST)
        aoo = boost::dynamic_pointer_cast<AssetOrNothingPayoff>(payoff);
        #else
        try {
            aoo = payoff;
        } catch (...) {}
        #endif
        if (!IsNull(aoo)) {
            // ok, the payoff is AssetOrNothingPayoff
            return "AssetOrNothingPayoff";
        }

        // SuperSharePayoff?
        Handle<SuperSharePayoff> ss;
        #if defined(HAVE_BOOST)
        ss = boost::dynamic_pointer_cast<SuperSharePayoff>(payoff);
        #else
        try {
            ss = payoff;
        } catch (...) {}
        #endif
        if (!IsNull(ss)) {
            // ok, the payoff is SuperSharePayoff
            return "SuperSharePayoff";
        }

        throw Error("payoffTypeToString : unknown payoff type");
    }


    std::string exerciseTypeToString(const Handle<Exercise>& exercise) {

        // EuropeanExercise?
        Handle<EuropeanExercise> european;
        #if defined(HAVE_BOOST)
        european = boost::dynamic_pointer_cast<EuropeanExercise>(exercise);
        #else
        try {
            european = exercise;
        } catch (...) {}
        #endif
        if (!IsNull(european)) {
            return "European";
        }

        // AmericanExercise?
        Handle<AmericanExercise> american;
        #if defined(HAVE_BOOST)
        american = boost::dynamic_pointer_cast<AmericanExercise>(exercise);
        #else
        try {
            american = exercise;
        } catch (...) {}
        #endif
        if (!IsNull(american)) {
            return "American";
        }

        // BermudanExercise?
        Handle<BermudanExercise> bermudan;
        #if defined(HAVE_BOOST)
        bermudan = boost::dynamic_pointer_cast<BermudanExercise>(exercise);
        #else
        try {
            bermudan = exercise;
        } catch (...) {}
        #endif
        if (!IsNull(bermudan)) {
            return "Bermudan";
        }

        throw Error("exerciseTypeToString : unknown exercise type");
    }

    void vanillaOptionTestFailed(std::string greekName,
                                 const Handle<StrikedTypePayoff>& payoff,
                                 const Handle<Exercise>& exercise,
                                 double s,
                                 double q,
                                 double r,
                                 Date today,
                                 double v,
                                 double expected,
                                 double calculated,
                                 double tolerance = Null<double>()) {
      CPPUNIT_FAIL(exerciseTypeToString(exercise) + " "
          + OptionTypeFormatter::toString(payoff->optionType()) +
          " option with "
          + payoffTypeToString(payoff) + ":\n"
          "    spot value: "
          + DoubleFormatter::toString(s) + "\n"
          "    strike:           "
          + DoubleFormatter::toString(payoff->strike()) +"\n"
          "    dividend yield:   "
          + DoubleFormatter::toString(q) + "\n"
          "    risk-free rate:   "
          + DoubleFormatter::toString(r) + "\n"
          "    reference date:   "
          + DateFormatter::toString(today) + "\n"
          "    maturity:         "
          + DateFormatter::toString(exercise->lastDate()) + "\n"
          "    volatility:       "
          + DoubleFormatter::toString(v) + "\n\n"
          "    expected   " + greekName + ": "
          + DoubleFormatter::toString(expected) + "\n"
          "    calculated " + greekName + ": "
          + DoubleFormatter::toString(calculated) + "\n"
          "    error:            "
          + DoubleFormatter::toString(QL_FABS(expected-calculated)) + "\n"
          + (tolerance==Null<double>() ? std::string("") :
          "    tolerance:        " + DoubleFormatter::toString(tolerance)));
    }

    struct VanillaOptionData {
        Option::Type type;
        double strike;
        double s;      // spot
        double q;      // dividend
        double r;      // risk-free rate
        Time t;        // time to maturity
        double v;      // volatility
        double result; // expected result
    };

    int timeToDays(Time t) {
        return int(t*360+0.5);
    }

}

// tests

void EuropeanOptionTest::testValues() {

    /* The data below are from
       "Option pricing formulas", E.G. Haug, McGraw-Hill 1998
    */
    VanillaOptionData values[] = {
      // pag 2-8
      //        type, strike,   spot,    q,    r,    t,  vol,   value
      { Option::Call,  65.00,  60.00, 0.00, 0.08, 0.25, 0.30,  2.1334},
      { Option::Put,   95.00, 100.00, 0.05, 0.10, 0.50, 0.20,  2.4648},
      { Option::Put,   19.00,  19.00, 0.10, 0.10, 0.75, 0.28,  1.7011},
      { Option::Call,  19.00,  19.00, 0.10, 0.10, 0.75, 0.28,  1.7011},
      { Option::Call,   1.60,   1.56, 0.08, 0.06, 0.50, 0.12,  0.0291},
      { Option::Put,   70.00,  75.00, 0.05, 0.10, 0.50, 0.35,  4.0870},
      // pag 24
      { Option::Call, 100.00,  90.00, 0.10, 0.10, 0.10, 0.15,  0.0205},
      { Option::Call, 100.00, 100.00, 0.10, 0.10, 0.10, 0.15,  1.8734},
      { Option::Call, 100.00, 110.00, 0.10, 0.10, 0.10, 0.15,  9.9413},
      { Option::Call, 100.00,  90.00, 0.10, 0.10, 0.10, 0.25,  0.3150},
      { Option::Call, 100.00, 100.00, 0.10, 0.10, 0.10, 0.25,  3.1217},
      { Option::Call, 100.00, 110.00, 0.10, 0.10, 0.10, 0.25, 10.3556},
      { Option::Call, 100.00,  90.00, 0.10, 0.10, 0.10, 0.35,  0.9474},
      { Option::Call, 100.00, 100.00, 0.10, 0.10, 0.10, 0.35,  4.3693},
      { Option::Call, 100.00, 110.00, 0.10, 0.10, 0.10, 0.35, 11.1381},
      { Option::Call, 100.00,  90.00, 0.10, 0.10, 0.50, 0.15,  0.8069},
      { Option::Call, 100.00, 100.00, 0.10, 0.10, 0.50, 0.15,  4.0232},
      { Option::Call, 100.00, 110.00, 0.10, 0.10, 0.50, 0.15, 10.5769},
      { Option::Call, 100.00,  90.00, 0.10, 0.10, 0.50, 0.25,  2.7026},
      { Option::Call, 100.00, 100.00, 0.10, 0.10, 0.50, 0.25,  6.6997},
      { Option::Call, 100.00, 110.00, 0.10, 0.10, 0.50, 0.25, 12.7857},
      { Option::Call, 100.00,  90.00, 0.10, 0.10, 0.50, 0.35,  4.9329},
      { Option::Call, 100.00, 100.00, 0.10, 0.10, 0.50, 0.35,  9.3679},
      { Option::Call, 100.00, 110.00, 0.10, 0.10, 0.50, 0.35, 15.3086},
      { Option::Put,  100.00,  90.00, 0.10, 0.10, 0.10, 0.15,  9.9210},
      { Option::Put,  100.00, 100.00, 0.10, 0.10, 0.10, 0.15,  1.8734},
      { Option::Put,  100.00, 110.00, 0.10, 0.10, 0.10, 0.15,  0.0408},
      { Option::Put,  100.00,  90.00, 0.10, 0.10, 0.10, 0.25, 10.2155},
      { Option::Put,  100.00, 100.00, 0.10, 0.10, 0.10, 0.25,  3.1217},
      { Option::Put,  100.00, 110.00, 0.10, 0.10, 0.10, 0.25,  0.4551},
      { Option::Put,  100.00,  90.00, 0.10, 0.10, 0.10, 0.35, 10.8479},
      { Option::Put,  100.00, 100.00, 0.10, 0.10, 0.10, 0.35,  4.3693},
      { Option::Put,  100.00, 110.00, 0.10, 0.10, 0.10, 0.35,  1.2376},
      { Option::Put,  100.00,  90.00, 0.10, 0.10, 0.50, 0.15, 10.3192},
      { Option::Put,  100.00, 100.00, 0.10, 0.10, 0.50, 0.15,  4.0232},
      { Option::Put,  100.00, 110.00, 0.10, 0.10, 0.50, 0.15,  1.0646},
      { Option::Put,  100.00,  90.00, 0.10, 0.10, 0.50, 0.25, 12.2149},
      { Option::Put,  100.00, 100.00, 0.10, 0.10, 0.50, 0.25,  6.6997},
      { Option::Put,  100.00, 110.00, 0.10, 0.10, 0.50, 0.25,  3.2734},
      { Option::Put,  100.00,  90.00, 0.10, 0.10, 0.50, 0.35, 14.4452},
      { Option::Put,  100.00, 100.00, 0.10, 0.10, 0.50, 0.35,  9.3679},
      { Option::Put,  100.00, 110.00, 0.10, 0.10, 0.50, 0.35,  5.7963},
      // pag27
      { Option::Call,  40.00,  42.00, 0.08, 0.04, 0.75, 0.35,  5.0975}
    };

    DayCounter dc = Actual360();
    Handle<SimpleQuote> spot(new SimpleQuote(0.0));
    Handle<SimpleQuote> qRate(new SimpleQuote(0.0));
    Handle<TermStructure> qTS = makeFlatCurve(qRate, dc);
    Handle<SimpleQuote> rRate(new SimpleQuote(0.0));
    Handle<TermStructure> rTS = makeFlatCurve(rRate, dc);
    Handle<SimpleQuote> vol(new SimpleQuote(0.0));
    Handle<BlackVolTermStructure> volTS = makeFlatVolatility(vol, dc);
    Handle<PricingEngine> engine(new AnalyticEuropeanEngine);

    Date today = Date::todaysDate();

    for (Size i=0; i<LENGTH(values); i++) {

        Handle<StrikedTypePayoff> payoff(new
            PlainVanillaPayoff(values[i].type, values[i].strike));
        Date exDate = today.plusDays(timeToDays(values[i].t));
        Handle<Exercise> exercise(new EuropeanExercise(exDate));

        spot ->setValue(values[i].s);
        qRate->setValue(values[i].q);
        rRate->setValue(values[i].r);
        vol  ->setValue(values[i].v);

        Handle<BlackScholesStochasticProcess> stochProcess(new
            BlackScholesStochasticProcess(
                 RelinkableHandle<Quote>(spot),
                 RelinkableHandle<TermStructure>(qTS),
                 RelinkableHandle<TermStructure>(rTS),
                 RelinkableHandle<BlackVolTermStructure>(volTS)));

        VanillaOption option(stochProcess, payoff, exercise, engine);

        double calculated = option.NPV();
        if (QL_FABS(calculated-values[i].result) > 1e-4) {
            vanillaOptionTestFailed("value", payoff, exercise, values[i].s, values[i].q,
                values[i].r, today, values[i].v, values[i].result, calculated,
                1e-4);
        }
    }

}



void EuropeanOptionTest::testGreekValues() {

    /* The data below are from
       "Option pricing formulas", E.G. Haug, McGraw-Hill 1998
       pag 11-16
    */
    VanillaOptionData values[] = {
        //        type, strike,   spot,    q,    r,        t,  vol,  value
        // delta
        { Option::Call, 100.00, 105.00, 0.10, 0.10, 0.500000, 0.36,  0.5946 },
        { Option::Put,  100.00, 105.00, 0.10, 0.10, 0.500000, 0.36, -0.3566 },
        // elasticity
        { Option::Put,  100.00, 105.00, 0.10, 0.10, 0.500000, 0.36, -4.8775 },
        // gamma
        { Option::Call,  60.00,  55.00, 0.00, 0.10, 0.750000, 0.30,  0.0278 },
        { Option::Put,   60.00,  55.00, 0.00, 0.10, 0.750000, 0.30,  0.0278 },
        // vega
        { Option::Call,  60.00,  55.00, 0.00, 0.10, 0.750000, 0.30, 18.9358 },
        { Option::Put,   60.00,  55.00, 0.00, 0.10, 0.750000, 0.30, 18.9358 },
        // theta
        { Option::Put,  405.00, 430.00, 0.05, 0.07, 1.0/12.0, 0.20,-31.1924 },
        // theta per day
        { Option::Put,  405.00, 430.00, 0.05, 0.07, 1.0/12.0, 0.20, -0.0855 },
        // rho
        { Option::Call,  75.00,  72.00, 0.00, 0.09, 1.000000, 0.19, 38.7325 },
        // dividendRho
        { Option::Put,  490.00, 500.00, 0.05, 0.08, 0.250000, 0.15, 42.2254 }
    };

    DayCounter dc = Actual360();
    Handle<SimpleQuote> spot(new SimpleQuote(0.0));
    Handle<SimpleQuote> qRate(new SimpleQuote(0.0));
    Handle<TermStructure> qTS = makeFlatCurve(qRate, dc);
    Handle<SimpleQuote> rRate(new SimpleQuote(0.0));
    Handle<TermStructure> rTS = makeFlatCurve(rRate, dc);
    Handle<SimpleQuote> vol(new SimpleQuote(0.0));
    Handle<BlackVolTermStructure> volTS = makeFlatVolatility(vol, dc);
    Handle<PricingEngine> engine(new AnalyticEuropeanEngine);
    Handle<BlackScholesStochasticProcess> stochProcess(new
        BlackScholesStochasticProcess(
             RelinkableHandle<Quote>(spot),
             RelinkableHandle<TermStructure>(qTS),
             RelinkableHandle<TermStructure>(rTS),
             RelinkableHandle<BlackVolTermStructure>(volTS)));


    Date today = Date::todaysDate();

    Handle<StrikedTypePayoff> payoff;
    Date exDate;
    Handle<Exercise> exercise;
    Handle<VanillaOption> option;
    double calculated;

    int i = -1;

    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->delta();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("delta", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);

    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->delta();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("delta", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);

    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->elasticity();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("elasticity", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);


    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->gamma();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("gamma", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);

    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->gamma();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("gamma", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);


    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->vega();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("vega", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);


    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->vega();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("vega", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);


    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->theta();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("theta", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);


    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->thetaPerDay();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("thetaPerDay", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);


    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->rho();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("rho", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);


    i++;
    payoff = Handle<StrikedTypePayoff>(new
        PlainVanillaPayoff(values[i].type, values[i].strike));
    exDate = today.plusDays(timeToDays(values[i].t));
    exercise = Handle<Exercise>(new EuropeanExercise(exDate));
    spot ->setValue(values[i].s);
    qRate->setValue(values[i].q);
    rRate->setValue(values[i].r);
    vol  ->setValue(values[i].v);
    option = Handle<VanillaOption>(new VanillaOption(
        stochProcess, payoff, exercise, engine));
    calculated = option->dividendRho();
    if (QL_FABS(calculated-values[i].result) > 1e-4)
        vanillaOptionTestFailed("dividendRho", payoff, exercise, values[i].s, values[i].q,
            values[i].r, today, values[i].v, values[i].result, calculated,
            1e-4);

}

void EuropeanOptionTest::testGreeks() {

    std::map<std::string,double> calculated, expected, tolerance;
    tolerance["delta"]  = 1.0e-5;
    tolerance["gamma"]  = 1.0e-5;
    tolerance["theta"]  = 1.0e-5;
    tolerance["rho"]    = 1.0e-5;
    tolerance["divRho"] = 1.0e-5;
    tolerance["vega"]   = 1.0e-5;

    Option::Type types[] = { Option::Call, Option::Put, Option::Straddle };
    double strikes[] = { 50.0, 99.5, 100.0, 100.5, 150.0 };
    double underlyings[] = { 100.0 };
    Rate qRates[] = { 0.04, 0.05, 0.06 };
    Rate rRates[] = { 0.01, 0.05, 0.15 };
    Time residualTimes[] = { 1.0, 2.0 };
    double vols[] = { 0.11, 0.50, 1.20 };

    DayCounter dc = Actual360();
    Handle<SimpleQuote> spot(new SimpleQuote(0.0));
    Handle<SimpleQuote> qRate(new SimpleQuote(0.0));
    Handle<TermStructure> qTS = makeFlatCurve(qRate, dc);
    Handle<SimpleQuote> rRate(new SimpleQuote(0.0));
    Handle<TermStructure> rTS = makeFlatCurve(rRate, dc);
    Handle<SimpleQuote> vol(new SimpleQuote(0.0));
    Handle<BlackVolTermStructure> volTS = makeFlatVolatility(vol, dc);

    Handle<StrikedTypePayoff> payoff;

    Date today = Date::todaysDate();

    for (Size i=0; i<LENGTH(types); i++) {
      for (Size j=0; j<LENGTH(strikes); j++) {
        for (Size k=0; k<LENGTH(residualTimes); k++) {
          Date exDate = today.plusDays(timeToDays(residualTimes[k]));
          Handle<Exercise> exercise(new EuropeanExercise(exDate));
          Date exDateP = exDate.plusDays(1),
               exDateM = exDate.plusDays(-1);
          Time dT = (exDateP-exDateM)/360.0;
          for (Size kk=0; kk<4; kk++) {
              // option to check
              if (kk==0) {
                  payoff = Handle<StrikedTypePayoff>(new
                    PlainVanillaPayoff(types[i], strikes[j]));
              } else if (kk==1) {
                  payoff = Handle<StrikedTypePayoff>(new
                    CashOrNothingPayoff(types[i], strikes[j],
                    100.0));
              } else if (kk==2) {
                  payoff = Handle<StrikedTypePayoff>(new
                    AssetOrNothingPayoff(types[i], strikes[j]));
              } else if (kk==3) {
                  payoff = Handle<StrikedTypePayoff>(new
                    GapPayoff(types[i], strikes[j], 100.0));
              }
              Handle<VanillaOption> option = makeOption(payoff, exercise,
                  spot, qTS, rTS,volTS);
              // time-shifted exercise dates and options
              Handle<Exercise> exerciseP(new EuropeanExercise(exDateP));
              Handle<VanillaOption> optionP =
                  makeOption(payoff,exerciseP, spot,
                                     qTS,rTS,volTS);
              Handle<Exercise> exerciseM(new EuropeanExercise(exDateM));
              Handle<VanillaOption> optionM =
                  makeOption(payoff, exerciseM, spot,
                                     qTS, rTS, volTS);

              for (Size l=0; l<LENGTH(underlyings); l++) {
                for (Size m=0; m<LENGTH(qRates); m++) {
                  for (Size n=0; n<LENGTH(rRates); n++) {
                    for (Size p=0; p<LENGTH(vols); p++) {
                      double u = underlyings[l],
                             q = qRates[m],
                             r = rRates[n],
                             v = vols[p];
                      spot->setValue(u);
                      qRate->setValue(q);
                      rRate->setValue(r);
                      vol->setValue(v);

                      double value         = option->NPV();
                      calculated["delta"]  = option->delta();
                      calculated["gamma"]  = option->gamma();
                      calculated["theta"]  = option->theta();
                      calculated["rho"]    = option->rho();
                      calculated["divRho"] = option->dividendRho();
                      calculated["vega"]   = option->vega();

                      if (value > spot->value()*1.0e-5) {
                          // perturb spot and get delta and gamma
                          double du = u*1.0e-4;
                          spot->setValue(u+du);
                          double value_p = option->NPV(),
                                 delta_p = option->delta();
                          spot->setValue(u-du);
                          double value_m = option->NPV(),
                                 delta_m = option->delta();
                          spot->setValue(u);
                          expected["delta"] = (value_p - value_m)/(2*du);
                          expected["gamma"] = (delta_p - delta_m)/(2*du);

                          // perturb rates and get rho and dividend rho
                          double dr = r*1.0e-4;
                          rRate->setValue(r+dr);
                          value_p = option->NPV();
                          rRate->setValue(r-dr);
                          value_m = option->NPV();
                          rRate->setValue(r);
                          expected["rho"] = (value_p - value_m)/(2*dr);

                          double dq = q*1.0e-4;
                          qRate->setValue(q+dq);
                          value_p = option->NPV();
                          qRate->setValue(q-dq);
                          value_m = option->NPV();
                          qRate->setValue(q);
                          expected["divRho"] = (value_p - value_m)/(2*dq);

                          // perturb volatility and get vega
                          double dv = v*1.0e-4;
                          vol->setValue(v+dv);
                          value_p = option->NPV();
                          vol->setValue(v-dv);
                          value_m = option->NPV();
                          vol->setValue(v);
                          expected["vega"] = (value_p - value_m)/(2*dv);

                          // get theta from time-shifted options
                          expected["theta"] = (optionM->NPV() - optionP->NPV())/dT;

                          // compare
                          std::map<std::string,double>::iterator it;
                          for (it = calculated.begin(); 
                               it != calculated.end(); ++it) {
                              std::string greek = it->first;
                              double expct = expected  [greek],
                                     calcl = calculated[greek],
                                     tol   = tolerance [greek];
                              if (relativeError(expct,calcl,u) > tol) {
                                  vanillaOptionTestFailed(greek, payoff, exercise, u, q, r, today,
                                      v, expct, calcl, tol);
                              }
                          }
                      }
                    }
                  }
                }
              }
            }
        }
      }
    }
}

void EuropeanOptionTest::testImpliedVol() {
    Size maxEvaluations = 100;
    double tolerance = 1.0e-6;

    // test options
    Option::Type types[] = { Option::Call, Option::Put, Option::Straddle };
    double strikes[] = { 50.0, 99.5, 100.0, 100.5, 150.0 };
    int lengths[] = { 36, 180, 360, 1080 };

    // test data
    double underlyings[] = { 80.0, 95.0, 99.9, 100.0, 100.1, 105.0, 120.0 };
    Rate qRates[] = { 0.01, 0.05, 0.10 };
    Rate rRates[] = { 0.01, 0.05, 0.10 };
    double vols[] = { 0.01, 0.20, 0.30, 0.70, 0.90 };

    // will fail with Actual 360. Why ??????????????????????
//    DayCounter dc = Actual360();
    DayCounter dc = Actual365();

    Handle<SimpleQuote> spot(new SimpleQuote(0.0));
    Handle<SimpleQuote> vol(new SimpleQuote(0.0));
    Handle<BlackVolTermStructure> volTS = makeFlatVolatility(vol,dc);
    Handle<SimpleQuote> qRate(new SimpleQuote(0.0));
    Handle<TermStructure> qTS = makeFlatCurve(qRate, dc);
    Handle<SimpleQuote> rRate(new SimpleQuote(0.0));
    Handle<TermStructure> rTS = makeFlatCurve(rRate, dc);

    Date today = Date::todaysDate();

    for (Size i=0; i<LENGTH(types); i++) {
      for (Size j=0; j<LENGTH(strikes); j++) {
        for (Size k=0; k<LENGTH(lengths); k++) {
          // option to check
          Date exDate = today.plusDays(lengths[k]);
          Handle<Exercise> exercise(new EuropeanExercise(exDate));
          Handle<StrikedTypePayoff> payoff(new
              PlainVanillaPayoff(types[i], strikes[j]));
          Handle<VanillaOption> option =
              makeOption(payoff, exercise, spot,
                                 qTS, rTS, volTS);

          for (Size l=0; l<LENGTH(underlyings); l++) {
            for (Size m=0; m<LENGTH(qRates); m++) {
              for (Size n=0; n<LENGTH(rRates); n++) {
                for (Size p=0; p<LENGTH(vols); p++) {
                  double u = underlyings[l],
                         q = qRates[m],
                         r = rRates[n],
                         v = vols[p];
                  spot->setValue(u);
                  qRate->setValue(q);
                  rRate->setValue(r);
                  vol->setValue(v);

                  double value = option->NPV();
                  double implVol = 0.0; // just to remove a warning...
                  if (value != 0.0) {
                      // shift guess somehow
                      vol->setValue(v*1.5);
                      try {
                          implVol = option->impliedVolatility(value,
                                                              tolerance,
                                                              maxEvaluations);
                      } catch (std::exception& e) {
                          CPPUNIT_FAIL(
                              OptionTypeFormatter::toString(types[i])
                              + " option :\n"
                              "    spot value: "
                              + DoubleFormatter::toString(u) + "\n"
                              "    strike:           "
                              + DoubleFormatter::toString(strikes[j]) +"\n"
                              "    dividend yield:   "
                              + DoubleFormatter::toString(q) + "\n"
                              "    risk-free rate:   "
                              + DoubleFormatter::toString(r) + "\n"
                              "    maturity:         "
                              + DateFormatter::toString(exDate) + "\n"
                              "    volatility:       "
                              + DoubleFormatter::toString(v) + "\n\n"
                              + std::string(e.what()));
                      }
                      if (QL_FABS(implVol-v) > tolerance) {
                          // the difference might not matter
                          vol->setValue(implVol);
                          double value2 = option->NPV();
                          if (relativeError(value,value2,u) > tolerance) {
                              CPPUNIT_FAIL(
                                  OptionTypeFormatter::toString(types[i])
                                  + " option :\n"
                                  "    spot value: "
                                  + DoubleFormatter::toString(u) + "\n"
                                  "    strike:           "
                                  + DoubleFormatter::toString(strikes[j]) +"\n"
                                  "    dividend yield:   "
                                  + DoubleFormatter::toString(q) + "\n"
                                  "    risk-free rate:   "
                                  + DoubleFormatter::toString(r) + "\n"
                                  "    maturity:         "
                                  + DateFormatter::toString(exDate) + "\n\n"
                                  "    original volatility: "
                                  + DoubleFormatter::toString(v) + "\n"
                                  "    price:               "
                                  + DoubleFormatter::toString(value) + "\n"
                                  "    implied volatility:  "
                                  + DoubleFormatter::toString(implVol) + "\n"
                                  "    corresponding price: "
                                  + DoubleFormatter::toString(value2));
                          }
                      }
                  }
                }
              }
            }
          }
        }
      }
    }
}

// different engines

namespace {

    void testEngineConsistency(EngineType *engines, Size N) {

        double tolerance = 0.03;

        // test options
        Option::Type types[] = { Option::Call, Option::Put, Option::Straddle };
        double strikes[] = { 50.0, 100.0, 150.0 };
        int lengths[] = { 1 };

        // test data
        double underlyings[] = { 100.0 };
        Rate qRates[] = { 0.00, 0.05 };
        Rate rRates[] = { 0.01, 0.05, 0.15 };
        double vols[] = { 0.11, 0.50, 1.20 };

        DayCounter dc = Actual360();
        Handle<SimpleQuote> spot(new SimpleQuote(0.0));
        Handle<SimpleQuote> vol(new SimpleQuote(0.0));
        Handle<BlackVolTermStructure> volTS = makeFlatVolatility(vol, dc);
        Handle<SimpleQuote> qRate(new SimpleQuote(0.0));
        Handle<TermStructure> qTS = makeFlatCurve(qRate, dc);
        Handle<SimpleQuote> rRate(new SimpleQuote(0.0));
        Handle<TermStructure> rTS = makeFlatCurve(rRate, dc);

        Date today = Date::todaysDate();

        for (Size i=0; i<LENGTH(types); i++) {
          for (Size j=0; j<LENGTH(strikes); j++) {
            for (Size k=0; k<LENGTH(lengths); k++) {
              Date exDate = today.plusDays(lengths[k]*360);
              Handle<Exercise> exercise(new EuropeanExercise(exDate));
              Handle<StrikedTypePayoff> payoff(new
                  PlainVanillaPayoff(types[i], strikes[j]));
              // reference option
              Handle<VanillaOption> refOption =
                  makeOption(payoff, exercise, spot,
                                     qTS, rTS, volTS);
              // options to check
              std::map<EngineType,Handle<VanillaOption> > options;
              for (Size ii=0; ii<N; ii++) {
                  options[engines[ii]] =
                      makeOption(payoff, exercise, spot,
                                         qTS, rTS, volTS,
                                         engines[ii]);
              }

              for (Size l=0; l<LENGTH(underlyings); l++) {
                for (Size m=0; m<LENGTH(qRates); m++) {
                  for (Size n=0; n<LENGTH(rRates); n++) {
                    for (Size p=0; p<LENGTH(vols); p++) {
                      double u = underlyings[l],
                             q = qRates[m],
                             r = rRates[n],
                             v = vols[p];
                      spot->setValue(u);
                      qRate->setValue(q);
                      rRate->setValue(r);
                      vol->setValue(v);

                      double refValue = refOption->NPV();
                      for (Size ii=0; ii<N; ii++) {
                          double value = options[engines[ii]]->NPV();
                          if (relativeError(value,refValue,u) > tolerance) {
                              CPPUNIT_FAIL("European "
                                  + OptionTypeFormatter::toString(types[i]) +
                                  " option :\n"
                                  "    spot value: "
                                  + DoubleFormatter::toString(u) + "\n"
                                  "    strike:           "
                                  + DoubleFormatter::toString(strikes[j]) +"\n"
                                  "    dividend yield:   "
                                  + DoubleFormatter::toString(q) + "\n"
                                  "    risk-free rate:   "
                                  + DoubleFormatter::toString(r) + "\n"
                                  "    reference date:   "
                                  + DateFormatter::toString(today) + "\n"
                                  "    maturity:         "
                                  + DateFormatter::toString(exDate) + "\n"
                                  "    volatility:       "
                                  + DoubleFormatter::toString(v) + "\n\n"
                                  "    analytic value: "
                                  + DoubleFormatter::toString(refValue) + "\n"
                                  "    "
                                  + engineTypeToString(engines[ii]) + ":  "
                                  + DoubleFormatter::toString(value));
                          }
                      }
                    }
                  }
                }
              }
            }
          }
        }
    }

}

void EuropeanOptionTest::testBinomialEngines() {
    EngineType engines[] = { JR, CRR, EQP, TGEO, TIAN, LR };
    testEngineConsistency(engines,LENGTH(engines));
}

void EuropeanOptionTest::testMcEngines() {
    EngineType engines[] = { PseudoMonteCarlo, QuasiMonteCarlo };
    testEngineConsistency(engines,LENGTH(engines));
}


CppUnit::Test* EuropeanOptionTest::suite() {
    CppUnit::TestSuite* tests =
        new CppUnit::TestSuite("European option tests");

    tests->addTest(new CppUnit::TestCaller<EuropeanOptionTest>
                   ("Testing European option values",
                    &EuropeanOptionTest::testValues));
    tests->addTest(new CppUnit::TestCaller<EuropeanOptionTest>
                   ("Testing European option greek values",
                    &EuropeanOptionTest::testGreekValues));

    tests->addTest(new CppUnit::TestCaller<EuropeanOptionTest>
                   ("Testing European option greeks",
                    &EuropeanOptionTest::testGreeks));

    tests->addTest(new CppUnit::TestCaller<EuropeanOptionTest>
                   ("Testing European option implied volatility",
                    &EuropeanOptionTest::testImpliedVol));

    tests->addTest(new CppUnit::TestCaller<EuropeanOptionTest>
                   ("Testing binomial European engines "
                    "against analytic results",
                    &EuropeanOptionTest::testBinomialEngines));
    tests->addTest(new CppUnit::TestCaller<EuropeanOptionTest>
                   ("Testing Monte Carlo European engines "
                    "against analytic results",
                    &EuropeanOptionTest::testMcEngines));

    return tests;
}

