
/*
 Copyright (C) 2003 Neil Firth

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

/*! \file americanmcengines.hpp
    \brief Least-square Monte Carlo engines
*/

#ifndef quantlib_american_montecarlo_engine_h
#define quantlib_american_montecarlo_engine_h

#include <ql/grid.hpp>
#include <ql/Math/svd.hpp>
#include <ql/MonteCarlo/mctypedefs.hpp>
#include <ql/MonteCarlo/europeanpathpricer.hpp>
#include <ql/PricingEngines/vanillaengines.hpp>

namespace QuantLib {

    typedef std::vector<std::vector<double> > AssetGrid;

    typedef std::vector<std::vector<double> > PayoffGrid;

    //! least-square Monte Carlo engine
    /*! \unstable This engine is known not to work for
                  deeply out-of-the-money options. 
                  More problems might surface.
    */
    class AmericanMCVanillaEngine : public VanillaEngine {
      public:
        AmericanMCVanillaEngine(Size requiredSamples,
                                Size timeSteps,
                                long seed = 0);
        void calculate() const;
      private:
        Size requiredSamples_;
        Size timeSteps_;
        long seed_;
    };

    // constructor
    inline
    AmericanMCVanillaEngine::AmericanMCVanillaEngine(Size requiredSamples,
                                                     Size timeSteps,
                                                     long seed) 
    : requiredSamples_(requiredSamples), timeSteps_(timeSteps),
      seed_(seed) {}

    // put all the asset prices into a vector.
    // s0 is not included in the vector
    std::vector<double> getAssetSequence(double s0, const Path& path);

    // get the Longstaff-Schwartz exampe asset prices
    // s0 is not included in the vector
    void getLSAssetsExample(AssetGrid& assetPaths, int timeSteps);

}


#endif
