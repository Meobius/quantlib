
/*
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

/*! \file montecarlomodel.hpp
    \brief General purpose Monte Carlo model
*/

#ifndef quantlib_montecarlo_model_h
#define quantlib_montecarlo_model_h

#include <ql/handle.hpp>
#include <ql/MonteCarlo/mctraits.hpp>
#include <ql/Math/statistics.hpp>

namespace QuantLib {

    //! Monte Carlo framework
    /*! See sect. \ref mcarlo */
    namespace MonteCarlo {

        //! General purpose Monte Carlo model for path samples
        /*! The template arguments of this class correspond to
            available policies for the particular model to be
            instantiated---i.e., whether it is single- or multi-asset,
            or whether it should use pseudo-random or low-discrepancy
            numbers for path generation. Such decisions are grouped
            in trait classes so as to be orthogonal---see mctraits.hpp
            for examples.

            The constructor accepts two safe references, i.e. two smart
            pointers, one to a path generator and the other to a path pricer.
            In case of control variate technique the user should provide the
            additional control option, namely the option path pricer and the
            option value.

        */
        template <class mc_traits,
                  class stats_traits = Math::Statistics>
        class MonteCarloModel {
          public:
            // extract traits
            typedef typename mc_traits::rsg_type rsg_type;
            typedef typename mc_traits::path_generator_type 
                path_generator_type;
            typedef typename mc_traits::path_pricer_type path_pricer_type;
            typedef typename path_generator_type::sample_type sample_type;
            typedef typename path_pricer_type::result_type result_type;
            typedef stats_traits stats_type;
            // constructor
            MonteCarloModel(const Handle<path_generator_type>& pathGenerator,
                            const Handle<path_pricer_type>& pathPricer,
                            const stats_type& sampleAccumulator,
                            bool antitheticVariate,
                            const Handle<path_pricer_type>& cvPathPricer 
                                = Handle<path_pricer_type>(),
                            result_type cvOptionValue = result_type())
            : pathGenerator_(pathGenerator), pathPricer_(pathPricer),
              sampleAccumulator_(sampleAccumulator),
              isAntitheticVariate_(antitheticVariate),
              cvPathPricer_(cvPathPricer), cvOptionValue_(cvOptionValue) {
                if (cvPathPricer_.isNull())
                    isControlVariate_=false;
                else
                    isControlVariate_=true;
            }
            void addSamples(Size samples);
            const stats_type& sampleAccumulator(void) const;
          private:
            Handle<path_generator_type> pathGenerator_;
            Handle<path_pricer_type> pathPricer_;
            stats_type sampleAccumulator_;
            bool isAntitheticVariate_;
            Handle<path_pricer_type> cvPathPricer_;
            result_type cvOptionValue_;
            bool isControlVariate_;
        };

        // inline definitions
        template<class MC, class S>
        inline void MonteCarloModel<MC,S>::addSamples(Size samples) {
            for(Size j = 1; j <= samples; j++) {

                sample_type path = pathGenerator_->next();
                result_type price = (*pathPricer_)(path.value);

                if (isControlVariate_)
                    price += cvOptionValue_-(*cvPathPricer_)(path.value);

                if (isAntitheticVariate_) {
                    path = pathGenerator_->antithetic();
                    result_type price2 = (*pathPricer_)(path.value);
                    if (isControlVariate_)
                        price2 += cvOptionValue_-(*cvPathPricer_)(path.value);
                    sampleAccumulator_.add((price+price2)/2.0, path.weight);
                } else {
                    sampleAccumulator_.add(price, path.weight);
                }
            }
        }

        template<class MC, class S>
        inline const typename MonteCarloModel<MC,S>::stats_type& 
        MonteCarloModel<MC,S>::sampleAccumulator() const {
            return sampleAccumulator_;
        }

    }

}


#endif
