
/*
 Copyright (C) 2003 Ferdinando Ametrano
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

/*! \file diffusionprocess.cpp
    \brief Diffusion process
*/

#include <ql/diffusionprocess.hpp>
#include <ql/Volatilities/localvolsurface.hpp>
#include <ql/Volatilities/localvolcurve.hpp>
#include <ql/Volatilities/localconstantvol.hpp>

namespace QuantLib {

    BlackScholesProcess::BlackScholesProcess(
                    const RelinkableHandle<TermStructure>& riskFreeTS,
                    const RelinkableHandle<TermStructure>& dividendTS,
                    const RelinkableHandle<BlackVolTermStructure>& blackVolTS,
                    double s0)
    : DiffusionProcess(s0), riskFreeTS_(riskFreeTS),
      dividendTS_(dividendTS) {

        // constant Black vol?
        Handle<BlackConstantVol> constVol;
        try {
            constVol = (*blackVolTS).currentLink();
        } catch (...) {}
        if (!constVol.isNull()) {
            // ok, the local vol is constant too.
            localVolTS_ = RelinkableHandle<LocalVolTermStructure>(
                Handle<LocalVolTermStructure>(
                    new LocalConstantVol(constVol->referenceDate(),
                                         constVol->blackVol(0.0, s0),
                                         constVol->dayCounter())));
            return;
        }

        // ok, so it's not constant. Maybe it's strike-independent?
        Handle<BlackVarianceCurve> volCurve;
        try {
            volCurve = (*blackVolTS).currentLink();
        } catch (...) {}
        if (!volCurve.isNull()) {
            // ok, we can use the optimized algorithm
            localVolTS_ = RelinkableHandle<LocalVolTermStructure>(
                Handle<LocalVolTermStructure>(
                    new LocalVolCurve(
                        RelinkableHandle<BlackVarianceCurve>(volCurve))));
            return;
        }

        // ok, so it's strike-dependent. Never mind.
        localVolTS_ = RelinkableHandle<LocalVolTermStructure>(
            Handle<LocalVolTermStructure>(
                new LocalVolSurface(blackVolTS, riskFreeTS,
                                    dividendTS, s0)));
    }

    double BlackScholesProcess::drift(Time t, double x) const {
        // this is a quick and dirty patch
        // rethink how to handle extrapolation
        double sigma = localVolTS_->localVol(t, x, true);

        // we could be more anticipatory if we know the right dt
        // for which the drift will be used
        double t1 = t + 0.0001;
        return riskFreeTS_->forward(t, t1)
            - dividendTS_->forward(t, t1)
            - 0.5 * sigma * sigma;
    }
}
