
/*
 Copyright (C) 2003 Ferdinando Ametrano

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

/*! \file blackformula.hpp
    \brief Black formula
*/

#ifndef quantlib_blackformula_h
#define quantlib_blackformula_h

#include <ql/Instruments/payoffs.hpp>
#include <ql/Math/normaldistribution.hpp>

namespace QuantLib {

    class BlackFormula {
    public:
        BlackFormula(double forward,
                     double discount,
                     double variance,
                     const Handle<StrikedTypePayoff>& payoff);
        double value() const;
        double delta(double spot) const;
        double elasticity(double spot) const;
        double gamma(double spot) const;
        double deltaForward() const;
        double elasticityForward() const;
        double gammaForward() const;
        double theta(double spot, double maturity) const;
        double thetaPerDay(double spot, double maturity) const;
        double vega(double maturity) const;
        double rho(double maturity) const;
        double dividendRho(double maturity) const;
        double itmProbability() const;
        double strikeSensitivity() const;
    private:
        double forward_, discount_, variance_;
        Handle<StrikedTypePayoff> payoff_;

        double stdDev_, strike_;
        double D1_, D2_, alpha_, beta_, DalphaDd1_, DbetaDd2_;
        double cum_d2_;
        double X_, DXDs_, DXDstrike_;
    };


    inline BlackFormula::BlackFormula(double forward, double discount,
        double variance, const Handle<StrikedTypePayoff>& payoff)
    : forward_(forward), discount_(discount), variance_(variance),
      payoff_(payoff) {

        QL_REQUIRE(forward>0.0,
            "BlackFormula::BlackFormula : "
            "positive forward value required");

        QL_REQUIRE(discount>0.0,
            "BlackFormula::BlackFormula : "
            "positive discount required");

        QL_REQUIRE(variance>=0.0,
            "BlackFormula::BlackFormula : "
            "negative variance not allowed");

        stdDev_ = QL_SQRT(variance);
        strike_ = payoff->strike();


        double cum_d1, n_d1, n_d2;
        if (variance>=QL_EPSILON) {
            if (strike_==0.0) {
                n_d1 = 0.0;
                n_d2 = 0.0;
                cum_d1 = 1.0;
                cum_d2_= 1.0;
            } else {
                CumulativeNormalDistribution f;
                D1_ = (QL_LOG(forward/strike_) +
                             0.5 * variance) / stdDev_;
                D2_ = D1_-stdDev_;
                cum_d1 = f(D1_);
                cum_d2_= f(D2_);
                n_d1 = f.derivative(D1_);
                n_d2 = f.derivative(D2_);
            }
        } else {
            n_d1 = 0.0;
            n_d2 = 0.0;
            if (forward>strike_) {
                cum_d1 = 1.0;
                cum_d2_= 1.0;
            } else {
                cum_d1 = 0.0;
                cum_d2_= 0.0;
            }
        }

        X_ = strike_;
        DXDstrike_ = 1.0;

        // the following one will probably disappear as soon as
        // super-share will be properly handled
        DXDs_ = 0.0;

        // Plain Vanilla Payoff
        switch (payoff->optionType()) {
            case Option::Call:
                alpha_     =  cum_d1; //  N(d1)
                DalphaDd1_ =    n_d1; //  n(d1)
                beta_      = -cum_d2_;// -N(d2)
                DbetaDd2_  = -  n_d2; // -n(d2)
                break;
            case Option::Put:
                alpha_     = -1.0+cum_d1; // -N(-d1)
                DalphaDd1_ =        n_d1; //  n( d1)
                beta_      =  1.0-cum_d2_;//  N(-d2)
                DbetaDd2_  =     -  n_d2; // -n( d2)
                break;
            case Option::Straddle:
                // incorporating the linear effect of call + put
                alpha_     = -1.0 + 2.0*cum_d1; //  N(d1) - N(-d1)
                DalphaDd1_ =        2.0*  n_d1; //  n(d1) + n( d1)
                beta_      =  1.0 - 2.0*cum_d2_;// -N(d2) + N(-d2)
                DbetaDd2_  =      - 2.0*  n_d2; // -n(d2) - n( d2)
                break;
            default:
                throw IllegalArgumentError("BlackFormula::BlackFormula : "
                                       "invalid option type");
        }

        // Binary Cash-Or-Nothing payoff?
        Handle<CashOrNothingPayoff> coo;
        #if defined(HAVE_BOOST)
        coo = boost::dynamic_pointer_cast<CashOrNothingPayoff>(payoff_);
        #else
        try {
            coo = payoff_;
        } catch (...) {}
        #endif
        if (!IsNull(coo)) {
            // ok, the payoff is Binary Cash-Or-Nothing
            alpha_ = DalphaDd1_ = 0.0;
            X_ = coo->cashPayoff();
            DXDstrike_ = 0.0;
            switch (payoff->optionType()) {
                case Option::Call:
                    beta_     = cum_d2_;// N(d2)
                    DbetaDd2_ =   n_d2; // n(d2)
                    break;
                case Option::Put:
                    beta_     = 1.0-cum_d2_;//  N(-d2)
                    DbetaDd2_ =    -  n_d2; // -n( d2)
                    break;
                case Option::Straddle:
                    // incorporating the linear effect of call + put
                    beta_     = 1.0; // N(d2) + N(-d2) = 1.0
                    DbetaDd2_ = 0.0; // n(d2) - n( d2) = 0.0
                    break;
                default:
                    throw IllegalArgumentError("BlackFormula::BlackFormula : "
                                           "invalid option type");
            }
        }

        // Binary Asset-Or-Nothing payoff?
        Handle<AssetOrNothingPayoff> aoo;
        #if defined(HAVE_BOOST)
        aoo = boost::dynamic_pointer_cast<AssetOrNothingPayoff>(payoff_);
        #else
        try {
            aoo = payoff_;
        } catch (...) {}
        #endif
        if (!IsNull(aoo)) {
            // ok, the payoff is Binary Asset-Or-Nothing
            beta_ = DbetaDd2_ = 0.0;
            switch (payoff->optionType()) {
                case Option::Call:
                    alpha_     =  cum_d1; //  N(d1)
                    DalphaDd1_ =    n_d1; //  n(d1)
                    break;
                case Option::Put:
                    alpha_     = 1.0-cum_d1; //  N(-d1)
                    DalphaDd1_ =    -  n_d1; // -n( d1)
                    break;
                case Option::Straddle:
                    // incorporating the linear effect of call + put
                    alpha_     = 1.0; //  N(d1) + N(-d1) = 1.0
                    DalphaDd1_ = 0.0; //  n(d1) - n( d1) = 0.0
                    break;
                default:
                    throw IllegalArgumentError("BlackFormula::BlackFormula : "
                                           "invalid option type");
            }
        }

        // Binary Gap payoff?
        Handle<GapPayoff> gap;
        #if defined(HAVE_BOOST)
        gap = boost::dynamic_pointer_cast<GapPayoff>(payoff_);
        #else
        try {
            gap = payoff_;
        } catch (...) {}
        #endif
        if (!IsNull(gap)) {
            // ok, the payoff is Binary Gap
            X_ = gap->strikePayoff();
            DXDstrike_ = 0.0;
        }

        // Binary Super-Share payoff?
        Handle<SuperSharePayoff> ss;
        #if defined(HAVE_BOOST)
        ss = boost::dynamic_pointer_cast<SuperSharePayoff>(payoff_);
        #else
        try {
            ss = payoff_;
        } catch (...) {}
        #endif
        if (!IsNull(ss)) {
            // ok, the payoff is Binary Super-Share
            throw Error("Binary Super-Share payoff not handled yet");
        }
    }



    
    inline double BlackFormula::value() const {
        return discount_ * (forward_ * alpha_ + X_ * beta_);
    }

    inline double BlackFormula::delta(double spot) const {

        QL_REQUIRE(spot>0.0,
            "BlackFormula::delta : "
            "positive spot value required");

        double DforwardDs = forward_ / spot;

        double temp = stdDev_*spot;
        double DalphaDs = DalphaDd1_/temp;
        double DbetaDs  = DbetaDd2_/temp;
        double temp2 = DalphaDs * forward_ + alpha_ * DforwardDs
                      +DbetaDs  * X_       + beta_  * DXDs_;

        return discount_ * temp2;
    }

    inline double BlackFormula::deltaForward() const {

        double temp = stdDev_*forward_;
        double DalphaDforward = DalphaDd1_/temp;
        double DbetaDforward  = DbetaDd2_/temp;
        double temp2 = DalphaDforward * forward_ + alpha_
                      +DbetaDforward  * X_; // DXDforward = 0.0

        return discount_ * temp2;
    }

    //! Sensitivity in percent to a percent movement in the underlying price
    inline double BlackFormula::elasticity(double spot) const {
        return delta(spot)/value()*spot;
    }

    //! Sensitivity in percent to a percent movement in the forward price
    inline double BlackFormula::elasticityForward() const {
        return deltaForward()/value()*forward_;
    }

    inline double BlackFormula::gamma(double spot) const {

        QL_REQUIRE(spot>0.0,
            "BlackFormula::gamma : "
            "positive spot value required");

        double DforwardDs = forward_ / spot;

        double temp = stdDev_*spot;
        double DalphaDs = DalphaDd1_/temp;
        double DbetaDs  = DbetaDd2_/temp;

        double D2alphaDs2 = - DalphaDs/spot*(1+D1_/stdDev_);
        double D2betaDs2  = - DbetaDs /spot*(1+D2_/stdDev_);

        double temp2 = D2alphaDs2 * forward_ + 2.0 * DalphaDs * DforwardDs
                      +D2betaDs2  * X_       + 2.0 * DbetaDs  * DXDs_;

        return  discount_ * temp2;
    }

    inline double BlackFormula::gammaForward() const {

        double temp = stdDev_*forward_;
        double DalphaDforward = DalphaDd1_/temp;
        double DbetaDforward  = DbetaDd2_/temp;

        double D2alphaDforward2 = - DalphaDforward/forward_*(1+D1_/stdDev_);
        double D2betaDforward2  = - DbetaDforward /forward_*(1+D2_/stdDev_);

        double temp2 = D2alphaDforward2 * forward_ + 2.0 * DalphaDforward
                      +D2betaDforward2  * X_; // DXDforward = 0.0

        return discount_ * temp2;
    }

    inline double BlackFormula::theta(double spot, double maturity) const {

        QL_REQUIRE(maturity>=0.0,
            "BlackFormula::theta : "
            "negative maturity not allowed");

        double rate = -QL_LOG(discount_)/maturity;
        double dividendRate = -QL_LOG(forward_ / spot * discount_)/maturity;

        double vol;
        if (stdDev_>0.0) vol = stdDev_ / QL_SQRT(maturity);
        else             vol = 0.0;

        return rate*value() - (rate-dividendRate)*spot*delta(spot)
            - 0.5*vol*vol*spot*spot*gamma(spot);
    }

    inline double BlackFormula::thetaPerDay(double spot, double maturity) const {
        return theta(spot, maturity)/365.0;
    }

    inline double BlackFormula::rho(double maturity) const {
        QL_REQUIRE(maturity>=0.0,
            "BlackFormula::rho : "
            "negative maturity not allowed");

        // actually DalphaDr / T
        double DalphaDr = DalphaDd1_/stdDev_;
        double DbetaDr  = DbetaDd2_/stdDev_;
        double temp = DalphaDr * forward_ + alpha_ * forward_ + DbetaDr * X_;

        return maturity * (discount_ * temp - value());
    }

    inline double BlackFormula::dividendRho(double maturity) const {
        QL_REQUIRE(maturity>=0.0,
            "BlackFormula::dividendRho : "
            "negative maturity not allowed");

        // actually DalphaDq / T
        double DalphaDq = -DalphaDd1_/stdDev_;
        double DbetaDq  = -DbetaDd2_/stdDev_;

        double temp = DalphaDq * forward_ - alpha_ * forward_ + DbetaDq * X_;

        return maturity * discount_ * temp;
            ;
    }

    inline double BlackFormula::vega(double maturity) const {
        QL_REQUIRE(maturity>=0.0,
            "BlackFormula::vega : "
            "negative maturity not allowed");

        double temp = QL_LOG(strike_/forward_)/variance_;
        // actually DalphaDsigma / SQRT(T)
        double DalphaDsigma = DalphaDd1_*(temp+0.5);
        double DbetaDsigma  = DbetaDd2_ *(temp-0.5);

        double temp2 = DalphaDsigma * forward_ + DbetaDsigma * X_;

        return discount_ * QL_SQRT(maturity) * temp2;

    }

    inline double BlackFormula::strikeSensitivity() const {

        double temp = stdDev_*strike_;
        double DalphaDstrike = -DalphaDd1_/temp;
        double DbetaDstrike  = -DbetaDd2_/temp;

        double temp2 =
            DalphaDstrike * forward_ + DbetaDstrike * X_ + beta_ * DXDstrike_;

        return discount_ * temp2;
    }

    inline double BlackFormula::itmProbability() const {
        return cum_d2_;
    }

}


#endif
