
/*
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

/*! \file mcdiscretearithmeticapo.hpp
    \brief Discrete Arithmetic Average Price Option
*/

#ifndef quantlib_pricers_mc_discrete_arithmetic_average_price_option_h
#define quantlib_pricers_mc_discrete_arithmetic_average_price_option_h

#include <ql/option.hpp>
#include <ql/types.hpp>
#include <ql/Pricers/mcpricer.hpp>
#include <ql/MonteCarlo/mctypedefs.hpp>

namespace QuantLib {

    //! example of Monte Carlo pricer using a control variate
    /*! \todo Continous Averaging version     */
    class McDiscreteArithmeticAPO 
        : public McPricer<SingleAsset_old<PseudoRandom_old> >{
      public:
        McDiscreteArithmeticAPO(Option::Type type,
                                double underlying,
                                double strike,
                                Spread dividendYield,
                                Rate riskFreeRate,
                                const std::vector<Time>& times,
                                double volatility,
                                bool antitheticVariance,
                                bool controlVariate,
                                long seed=0);
    };

}


#endif
