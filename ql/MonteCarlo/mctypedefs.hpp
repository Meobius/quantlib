
/*
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

/*! \file mctypedefs.hpp
    \brief Default choices for template instantiations
*/

#ifndef quantlib_mc_typedefs_h
#define quantlib_mc_typedefs_h

#include <ql/RandomNumbers/rngtypedefs.hpp>
#include <ql/MonteCarlo/montecarlomodel.hpp>

namespace QuantLib {

    namespace MonteCarlo {

        //! default choice for Gaussian path generator.
        typedef PathGenerator_old<RandomNumbers::GaussianRandomGenerator>
            GaussianPathGenerator_old;

        typedef PathGenerator<RandomNumbers::GaussianRandomSequenceGenerator>
            GaussianPathGenerator;

        //! default choice for Gaussian multi-path generator.
        typedef
        MultiPathGenerator_old<RandomNumbers::RandomArrayGenerator<
            RandomNumbers::GaussianRandomGenerator> >
            GaussianMultiPathGenerator;

        //! default choice for one-factor Monte Carlo model.
        typedef MonteCarloModel<SingleAsset_old<PseudoRandom_old> >
                                    OneFactorMonteCarloOption_old;

        //! default choice for multi-factor Monte Carlo model.
        typedef MonteCarloModel<MultiAsset_old<PseudoRandomSequence_old> >
                                    MultiFactorMonteCarloOption;

    }

}


#endif
