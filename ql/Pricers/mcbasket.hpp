
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

/*! \file mcbasket.hpp
    \brief simple example of multi-factor Monte Carlo pricer
*/

#ifndef quantlib_basket_pricer_h
#define quantlib_basket_pricer_h

#include <ql/Pricers/mcpricer.hpp>
#include <ql/MonteCarlo/mctypedefs.hpp>

namespace QuantLib {

    namespace Pricers {

        //! simple example of multi-factor Monte Carlo pricer
        class McBasket 
        : public McPricer<MonteCarlo::MultiAsset_old<
                    MonteCarlo::PseudoRandomSequence_old> > {
          public:
              McBasket(Option::Type type,
                       const std::vector<double>& underlying,
                       double strike,
                       const Array& dividendYield,
                       const Math::Matrix& covariance,
                       Rate riskFreeRate,
                       double residualTime,
                       bool antitheticVariance,
                       long seed=0);
        };

    }

}


#endif
