

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
/*! \file mcpricer.hpp
    \brief base class for Monte Carlo pricers

    \fullpath
    ql/Pricers/%mcpricer.hpp
*/

// $Id$

#ifndef quantlib_montecarlo_pricer_h
#define quantlib_montecarlo_pricer_h

#include <ql/dataformatters.hpp>
#include <ql/MonteCarlo/montecarlomodel.hpp>

namespace QuantLib {

    namespace Pricers {

        //! base class for Monte Carlo pricers
        /*! Eventually this class might be linked to the general tree of
            pricers, in order to have tools like impliedVolatility available.
            Also, it could, eventually, offer greeks methods.
            Deriving a class from McPricer gives an easy way to write
            a Monte Carlo Pricer.
            See McEuropean as example of one factor pricer,
            Basket as example of multi factor pricer.
        */

        template<class S, class PG, class PP>
        class McPricer {
          public:
            virtual ~McPricer() {}
            //! add samples until the required tolerance is reached
            double value(double tolerance,
                         Size maxSample = QL_MAX_INT) const;
            //! simulate a fixed number of samples
            double valueWithSamples(Size samples) const;
            //! error Estimated of the samples simulated so far
            double errorEstimate() const;
            //! access to the sample accumulator for more statistics
            const S& sampleAccumulator(void) const;
          protected:
            McPricer() {}
            mutable Handle<MonteCarlo::MonteCarloModel<S, PG, PP> > mcModel_;
            static const Size minSample_;
        };


        template<class S, class PG, class PP>
        const Size McPricer<S, PG, PP>::minSample_ = 100;

        // inline definitions
        template<class S, class PG, class PP>
        inline double McPricer<S, PG, PP>::value(double tolerance,
            Size maxSamples) const {

            Size sampleNumber =
                mcModel_->sampleAccumulator().samples();
            if (sampleNumber<minSample_) {
                mcModel_->addSamples(minSample_-sampleNumber);
                sampleNumber = mcModel_->sampleAccumulator().samples();
            }

            Size nextBatch;
            double order;
            double result = mcModel_->sampleAccumulator().mean();
            double accuracy = mcModel_->sampleAccumulator().errorEstimate()/
                result;
            while (accuracy > tolerance) {
                // conservative estimate of how many samples are needed 
                order = accuracy*accuracy/tolerance/tolerance;
                
                nextBatch = Size(
                    QL_MAX(sampleNumber*order*0.8-sampleNumber,
                    double(minSample_)));
                // do not exceed maxSamples
                nextBatch = QL_MIN(nextBatch, maxSamples-sampleNumber);
                QL_REQUIRE(nextBatch>0,
                    "max number of samples exceeded");

                sampleNumber += nextBatch;
                mcModel_->addSamples(nextBatch);
                result = mcModel_->sampleAccumulator().mean();
                accuracy = mcModel_->sampleAccumulator().errorEstimate()/
                    result;
            }

            return result;
        }


        template<class S, class PG, class PP>
        inline double McPricer<S, PG, PP>::valueWithSamples(Size samples)
            const {

            QL_REQUIRE(samples>=minSample_,
                "number of requested samples ("
                + IntegerFormatter::toString(samples) +
                ") lower than minSample_ ("
                + IntegerFormatter::toString(minSample_) +
                ")");

            Size sampleNumber =
                mcModel_->sampleAccumulator().samples();

            QL_REQUIRE(samples>=sampleNumber,
                "number of already simulated samples ("
                + IntegerFormatter::toString(sampleNumber) +
                ") greater than"
                "requested samples ("
                + IntegerFormatter::toString(samples) +
                ")");

            mcModel_->addSamples(samples-sampleNumber);

            return mcModel_->sampleAccumulator().mean();
        }


        template<class S, class PG, class PP>
        inline double McPricer<S, PG, PP>::errorEstimate() const {

            Size sampleNumber =
                mcModel_->sampleAccumulator().samples();

            QL_REQUIRE(sampleNumber>=minSample_,
                "number of simulated samples lower than minSample_");

            return mcModel_->sampleAccumulator().errorEstimate();
        }

        template<class S, class PG, class PP>
        inline const S& McPricer<S, PG, PP>::sampleAccumulator() const {

            return mcModel_->sampleAccumulator();
        }
    }
}

#endif
