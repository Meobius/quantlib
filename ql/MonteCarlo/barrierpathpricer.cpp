
/*
 Copyright (C) 2003 Neil Firth
 Copyright (C) 2003 Ferdinando Ametrano
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

/*! \file barrierpathpricer.cpp
    \brief path pricer for Barrier options
*/

#include <ql/MonteCarlo/barrierpathpricer.hpp>

namespace QuantLib {

    // constructor
    BarrierPathPricer::BarrierPathPricer(
                            Barrier::Type barrierType, 
                            double barrier, 
                            double rebate, 
                            Option::Type type,
                            double underlying, 
                            double strike,
                            const RelinkableHandle<TermStructure>& riskFreeTS,
                            const Handle<DiffusionProcess>& diffProcess,
                            UniformRandomSequenceGenerator sequenceGen)
    : PathPricer<Path>(riskFreeTS), underlying_(underlying),
      payoff_(type, strike), barrierType_(barrierType),
      barrier_(barrier), rebate_(rebate), diffProcess_(diffProcess),
      sequenceGen_(sequenceGen) {
        QL_REQUIRE(underlying>0.0,
                   "BarrierPathPricer: "
                   "underlying less/equal zero not allowed");
        QL_REQUIRE(strike>0.0,
                   "BarrierPathPricer: "
                   "strike less/equal zero not allowed");
        QL_REQUIRE(barrier>0.0,
                   "BarrierPathPricer: "
                   "barrier less/equal zero not allowed");
    }



    // price a path
    double BarrierPathPricer::operator()(const Path& path) const {
        Size n = path.size();
        QL_REQUIRE(n>0,
                   "BarrierPathPricer: the path cannot be empty");

        bool isOptionActive = false;
        double asset_price = underlying_;
        double new_asset_price;
        double x, y, vol;
        TimeGrid timeGrid = path.timeGrid();
        Time dt;
        double log_drift, log_random;
        Array u = sequenceGen_.nextSequence().value;
        Size i;

        switch (barrierType_) {
          case Barrier::DownIn:
            isOptionActive = false;
            for (i = 0; i < n; i++) {
                log_drift = path.drift()[i];
                log_random = path.diffusion()[i];
                new_asset_price = asset_price * QL_EXP(log_drift+log_random);
                // terminal or initial vol?                        
                vol = diffProcess_->diffusion(timeGrid.at(i),asset_price);
                dt = timeGrid.dt(i);

                x = QL_LOG (new_asset_price / asset_price);
                y = 0.5*(x - QL_SQRT (x*x - 2*vol*vol*dt*QL_LOG(u[i])));
                y = asset_price * QL_EXP (y);
                if (y <= barrier_) {
                    isOptionActive = true;
                }
                asset_price = new_asset_price;
            }
            break;
          case Barrier::UpIn:
            isOptionActive = false;
            for (i = 0; i < n; i++) {
                log_drift = path.drift()[i];
                log_random = path.diffusion()[i];
                new_asset_price = asset_price * QL_EXP(log_drift+log_random);
                // terminal or initial vol?                        
                vol = diffProcess_->diffusion(timeGrid.at(i),asset_price);
                dt = timeGrid.dt(i);

                x = QL_LOG (new_asset_price / asset_price);
                y = 0.5*(x + QL_SQRT (x*x - 2*vol*vol*dt*QL_LOG((1-u[i]))));
                y = asset_price * QL_EXP (y);
                if (y >= barrier_) {
                    isOptionActive = true;
                }
                asset_price = new_asset_price;
            }
            break;
          case Barrier::DownOut:
            isOptionActive = true;
            for (i = 0; i < n; i++) {
                log_drift = path.drift()[i];
                log_random = path.diffusion()[i];
                new_asset_price = asset_price * QL_EXP(log_drift+log_random);
                // terminal or initial vol?                        
                vol = diffProcess_->diffusion(timeGrid.at(i),asset_price);
                dt = timeGrid.dt(i);

                x = QL_LOG (new_asset_price / asset_price);
                y = 0.5*(x - QL_SQRT (x*x - 2*vol*vol*dt*QL_LOG(u[i])));
                y = asset_price * QL_EXP (y);
                if (y <= barrier_) {
                    isOptionActive = false;
                }
                asset_price = new_asset_price;
            }
            break;
          case Barrier::UpOut:
            isOptionActive = true;
            for (i = 0; i < n; i++) {
                log_drift = path.drift()[i];
                log_random = path.diffusion()[i];
                new_asset_price = asset_price * QL_EXP(log_drift+log_random);
                // terminal or initial vol?                        
                vol = diffProcess_->diffusion(timeGrid.at(i),asset_price);
                dt = timeGrid.dt(i);

                x = QL_LOG (new_asset_price / asset_price);
                y = 0.5*(x + QL_SQRT (x*x - 2*vol*vol*dt*QL_LOG((1-u[i]))));
                y = asset_price * QL_EXP (y);
                if (y >= barrier_) {
                    isOptionActive = false;
                }
                asset_price = new_asset_price;
            }
            break;
          default:
            throw Error("BarrierPathPricer: unknown BarrierType");
        }

        if (isOptionActive) {
            return payoff_(asset_price) *
                riskFreeTS_->discount(path.timeGrid().back());
        } else {
            return 0.0;
        }
    }

}

