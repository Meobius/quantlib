
/*!
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

//! $Id$

/*  This example computes profit and loss of a discrete interval hedging
    strategy and compares with the results of Derman & Kamal's (Goldman Sachs
    Equity Derivatives Research) Research Note: "When You Cannot Hedge
    Continuously: The Corrections to Black-Scholes"
    (http://www.gs.com/qs/doc/when_you_cannot_hedge.pdf)

    Suppose an option hedger sells an European option and receives the
    Black-Scholes value as the options premium.
    Then he follows a Black-Scholes hedging strategy, rehedging at discrete,
    evenly spaced time intervals as the underlying stock changes. At
    expiration, the hedger delivers the option payoff to the option holder,
    and unwinds the hedge. We are interested in understanding the final
    profit or loss of this strategy.

    If the hedger had followed the exact Black-Scholes replication strategy,
    re-hedging continuously as the underlying stock evolved towards its final
    value at expiration, then, no matter what path the stock took, the final
    P&L would be exactly zero. When the replication strategy deviates from
    the exact Black-Scholes method, the final P&L may deviate from zero. This
    deviation is called the replication error. When the hedger rebalances at
    discrete rather than continuous intervals, the hedge is imperfect and the
    replication is inexact. The more often hedging occurs, the smaller the
    replication error.

    We examine the range of possibilities, computing the replication error.
*/

// the only header you need to use QuantLib
#include <ql/quantlib.hpp>


// introducing the players ....

// Rate and Time are just double, but having their own types allows for
// a stronger check at compile time
using QuantLib::Rate;
using QuantLib::Time;

// ExercisePayoff is a helper function that calculates option payoff
using QuantLib::Payoff;

// Option is a helper class that holds the enumeration {Call, Put, Straddle}
using QuantLib::Option;

// Handle is the QuantLib way to have reference-counted objects
using QuantLib::Handle;

// the classic Black Scholes analytic solution for European Option
using QuantLib::Pricers::EuropeanOption;

// class for statistical analysis
using QuantLib::Math::Statistics;

// single Path of a random variable
// It contains the list of variations
using QuantLib::MonteCarlo::Path;

// the pricer computes final portfolio's value for each random variable path
using QuantLib::MonteCarlo::PathPricer_old;

// the path generator
using QuantLib::MonteCarlo::GaussianPathGenerator_old;

// the Montecarlo pricing model for option on a single asset
using QuantLib::MonteCarlo::OneFactorMonteCarloOption_old;

// to format the output of doubles
using QuantLib::DoubleFormatter;

/* The ReplicationError class carries out Monte Carlo simulations to evaluate
   the outcome (the replication error) of the discrete hedging strategy over
   different, randomly generated scenarios of future stock price evolution.
*/
class ReplicationError
{
public:
    ReplicationError(Option::Type type,
                     Time maturity,
                     double strike,
                     double s0,
                     double sigma,
                     Rate r)
    : maturity_(maturity), payoff_(type, strike), s0_(s0),
      sigma_(sigma), r_(r) {

        // value of the option
        EuropeanOption option = EuropeanOption(type, s0_, strike, 0.0, r_,
            maturity_, sigma_);
        std::cout << "Option value: " << option.value() << std::endl;

        // store option's vega, since Derman and Kamal's formula needs it
        vega_ = option.vega();

        std::cout << std::endl;
        std::cout <<
            "        |        | P&L  \t|  P&L    | Derman&Kamal | P&L"
            "      \t| P&L" << std::endl;

        std::cout <<
            "samples | trades | Mean \t| Std Dev | Formula      |"
            " skewness \t| kurt." << std::endl;

        std::cout << "---------------------------------"
            "----------------------------------------------" << std::endl;
    }

    // the actual replication error computation
    void compute(int nTimeSteps, int nSamples);
private:
    Time maturity_;
    Payoff payoff_;
    double s0_;
    double sigma_;
    Rate r_;
    double vega_;
};

// The key for the MonteCarlo simulation is to have a PathPricer_old that
// implements a value(const Path& path) method.
// This method prices the portfolio for each Path of the random variable
class ReplicationPathPricer : public PathPricer_old<Path>
{
  public:
    // real constructor
    ReplicationPathPricer(Option::Type type,
                          double underlying,
                          double strike,
                          Rate r,
                          Time maturity,
                          double sigma)
    : PathPricer_old<Path>(1.0, false), type_(type), underlying_(underlying),
      strike_(strike), r_(r), maturity_(maturity), sigma_(sigma) {
        QL_REQUIRE(strike_ > 0.0,
            "ReplicationPathPricer: strike must be positive");
        QL_REQUIRE(underlying_ > 0.0,
            "ReplicationPathPricer: underlying must be positive");
        QL_REQUIRE(r_ >= 0.0,
            "ReplicationPathPricer: risk free rate (r) must"
            " be positive or zero");
        QL_REQUIRE(maturity_ > 0.0,
            "ReplicationPathPricer: maturity must be positive");
        QL_REQUIRE(sigma_ >= 0.0,
            "ReplicationPathPricer: volatility (sigma)"
            " must be positive or zero");

    }
    // The value() method encapsulates the pricing code
    double operator()(const Path& path) const;

  private:
    Option::Type type_;
    double underlying_, strike_;
    Rate r_;
    Time maturity_;
    double sigma_;
};


// Compute Replication Error as in the Derman and Kamal's research note
int main(int argc, char* argv[])
{
    try {
        QL_IO_INIT

        Time maturity = 1./12.;   // 1 month
        double strike = 100;
        double underlying = 100;
        double volatility = 0.20; // 20%
        Rate riskFreeRate = 0.05; // 5%
        ReplicationError rp(Option::Call, maturity, strike, underlying,
                volatility, riskFreeRate);

        int scenarios = 50000;
        int hedgesNum;

        hedgesNum = 21;
        rp.compute(hedgesNum, scenarios);

        hedgesNum = 84;
        rp.compute(hedgesNum, scenarios);

        return 0;
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "unknown error" << std::endl;
        return 1;
    }
}


/* The actual computation of the Profit&Loss for each single path.

   In each scenario N rehedging trades spaced evenly in time over
   the life of the option are carried out, using the Black-Scholes
   hedge ratio.
*/
double ReplicationPathPricer::operator()(const Path& path) const
{

    // path is an instance of QuantLib::MonteCarlo::Path
    // It contains the list of variations.
    // It can be used as an array: it has a size() method
    int n = path.size();
    QL_REQUIRE(n>0,
        "ReplicationPathPricer: the path cannot be empty");

    // discrete hedging interval
    Time dt = maturity_/n;

    // For simplicity, we assume the stock pays no dividends.
    double stockDividendYield = 0.0;

    // let's start
    Time t = 0;

    // stock value at t=0
    double stock = underlying_;
    double stockLogGrowth = 0.0;

    // money account at t=0
    double money_account = 0.0;

    /************************/
    /*** the initial deal ***/
    /************************/
    // option fair price (Black-Scholes) at t=0
    EuropeanOption option = EuropeanOption(type_, stock, strike_,
        stockDividendYield, r_, maturity_, sigma_);
    // sell the option, cash in its premium
    money_account += option.value();
    // compute delta
    double delta = option.delta();
    // delta-hedge the option buying stock
    double stockAmount = delta;
    money_account -= stockAmount*stock;

    /**********************************/
    /*** hedging during option life ***/
    /**********************************/
    for(int step = 0; step < n-1; step++){

        // time flows
        t += dt;

        // accruing on the money account
        money_account *= QL_EXP( r_*dt );

        // stock growth:
        // path contains the list of Gaussian variations
        // and path[n] is the n-th variation
        stockLogGrowth += path[step];
        stock = underlying_*QL_EXP(stockLogGrowth);

        // recalculate option value at the current stock value,
        // and the current time to maturity
        EuropeanOption option = EuropeanOption(type_, stock, strike_,
            stockDividendYield, r_, maturity_-t, sigma_);

        // recalculate delta
        delta = option.delta();

        // re-hedging
        money_account -= (delta - stockAmount)*stock;
        stockAmount = delta;
    }

    /*************************/
    /*** option expiration ***/
    /*************************/
    // last accrual on my money account
    money_account *= QL_EXP( r_*dt );
    // last stock growth
    stockLogGrowth += path[n-1];
    stock = underlying_*QL_EXP(stockLogGrowth);

    // the hedger delivers the option payoff to the option holder
    double optionPayoff = Payoff(type_, strike_)(stock);
    money_account -= optionPayoff;

    // and unwinds the hedge selling his stock position
    money_account += stockAmount*stock;

    // final Profit&Loss
    return money_account;
}


// The computation over nSamples paths of the P&L distribution
void ReplicationError::compute(int nTimeSteps, int nSamples)
{
    QL_REQUIRE(nTimeSteps>0,
        "ReplicationError::compute : the number of steps must be > 0");

    // hedging interval
    // double tau = maturity_ / nTimeSteps;

    /* Black-Scholes framework: the underlying stock price evolves
       lognormally with a fixed known volatility that stays constant
       throughout time.
    */
    double drift = r_ - 0.5*sigma_*sigma_;

    // Black Scholes equation rules the path generator:
    // at each step the log of the stock
    // will have drift and sigma^2 variance
    Handle<GaussianPathGenerator_old> myPathGenerator(
        new GaussianPathGenerator_old(drift, sigma_*sigma_,
            maturity_, nTimeSteps));

    // The replication strategy's Profit&Loss is computed for each path
    // of the stock. The path pricer knows how to price a path using its
    // value() method
    Handle<PathPricer_old<Path> > myPathPricer =
        Handle<PathPricer_old<Path> >(new
            ReplicationPathPricer(payoff_.optionType(), s0_,
                payoff_.strike(), r_, maturity_, sigma_));

    // a statistic accumulator for the path-dependant Profit&Loss values
    Statistics statisticAccumulator;

    // The OneFactorMontecarloModel generates paths using myPathGenerator
    // each path is priced using myPathPricer
    // prices will be accumulated into statisticAccumulator
    OneFactorMonteCarloOption_old MCSimulation(myPathGenerator,
        myPathPricer, statisticAccumulator, false);

    // the model simulates nSamples paths
    MCSimulation.addSamples(nSamples);

    // the sampleAccumulator method of OneFactorMonteCarloOption_old
    // gives access to all the methods of statisticAccumulator
    double PLMean  = MCSimulation.sampleAccumulator().mean();
    double PLStDev = MCSimulation.sampleAccumulator().standardDeviation();
    double PLSkew  = MCSimulation.sampleAccumulator().skewness();
    double PLKurt  = MCSimulation.sampleAccumulator().kurtosis();

    // Derman and Kamal's formula
    double theorStD = QL_SQRT(M_PI/4/nTimeSteps)*vega_*sigma_;


    std::cout << nSamples << "\t| "
        << nTimeSteps << "\t | "
        << DoubleFormatter::toString(PLMean,   3) << " \t| "
        << DoubleFormatter::toString(PLStDev,  2) << " \t  | "
        << DoubleFormatter::toString(theorStD, 2) << " \t | "
        << DoubleFormatter::toString(PLSkew,   2) << " \t| "
        << DoubleFormatter::toString(PLKurt,   2) << std::endl;
}
