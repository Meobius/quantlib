
/*
 * Copyright (C) 2000-2001 QuantLib Group
 *
 * This file is part of QuantLib.
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.org/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated
 * in the QuantLib License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You should have received a copy of the license along with this file;
 * if not, please email quantlib-users@lists.sourceforge.net
 * The license is also available at http://quantlib.org/LICENSE.TXT
 *
 * The members of the QuantLib Group are listed in the Authors.txt file, also
 * available at http://quantlib.org/group.html
*/

/*! \file mcaveragepriceasian.cpp
    \brief Average price Asian option (control variate Monte Carlo pricing)

    \fullpath
    ql/Pricers/%mcaveragepriceasian.cpp
*/

// $Id$

#include "ql/Pricers/mcaveragepriceasian.hpp"
#include "ql/MonteCarlo/mctypedefs.hpp"
#include "ql/MonteCarlo/avgpriceasianpathpricer.hpp"
#include "ql/MonteCarlo/geometricasianpathpricer.hpp"
#include "ql/Pricers/geometricasianoption.hpp"

namespace QuantLib {

    namespace Pricers {

        using MonteCarlo::OneFactorMonteCarloOption;
        using MonteCarlo::PathPricer;
        using MonteCarlo::GaussianPathGenerator;
        using MonteCarlo::AveragePriceAsianPathPricer;
        using MonteCarlo::GeometricAsianPathPricer;

        McAveragePriceAsian::McAveragePriceAsian(Option::Type type,
            double underlying, double strike, Spread dividendYield,
            Rate riskFreeRate, const std::vector<Time>& times,
            double volatility, bool antitheticVariance, long seed) {

            QL_REQUIRE(times.size() >= 2,
                "McAveragePriceAsian: you must have at least 2 time-steps");
            //! Initialize the path generator
            double mu = riskFreeRate - dividendYield
                                     - 0.5 * volatility * volatility;

            Handle<GaussianPathGenerator> pathGenerator(
                new GaussianPathGenerator(mu, volatility*volatility, 
                    times, seed));

            double residualTime = times[times.size()-1];


            //! Initialize the pricer on the single Path
            Handle<PathPricer> spPricer(
                new AveragePriceAsianPathPricer(type, underlying, strike,
                    QL_EXP(-riskFreeRate*residualTime), antitheticVariance));

            Handle<PathPricer> controlVariateSpPricer(
                new GeometricAsianPathPricer(type, underlying, strike,
                    QL_EXP(-riskFreeRate*residualTime), antitheticVariance));

            double controlVariatePrice = GeometricAsianOption(type, 
                underlying, strike, dividendYield, riskFreeRate, 
                residualTime, volatility).value();

            //! Initialize the Monte Carlo model
            mcModel_ = Handle<MonteCarlo::MonteCarloModel<Math::Statistics, MonteCarlo::GaussianPathGenerator, MonteCarlo::PathPricer> > (
                new MonteCarlo::MonteCarloModel<Math::Statistics, MonteCarlo::GaussianPathGenerator, MonteCarlo::PathPricer> (
                pathGenerator, spPricer, Math::Statistics(),
                controlVariateSpPricer, controlVariatePrice));

        }

    }

}
