
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

/*! \file rngtypedefs.hpp
    \brief Default choices for template instantiations

    \fullpath
    ql/RandomNumbers/%rngtypedefs.hpp
*/

// $Id$

#ifndef quantlib_rng_typedefs_h
#define quantlib_rng_typedefs_h

//#include <ql/RandomNumbers/brodasoboluniformlds.hpp>
#include <ql/RandomNumbers/lecuyeruniformrng.hpp>
#include <ql/RandomNumbers/boxmullergaussianrng.hpp>

namespace QuantLib {

    namespace RandomNumbers {

        //! default choice for uniform random number generator.
        typedef LecuyerUniformRng UniformRandomGenerator;

        //! default choice for Gaussian random number generator.
        typedef BoxMullerGaussianRng<UniformRandomGenerator> GaussianRandomGenerator;

        // default choice for uniform low discepancy sequence generator.
//        typedef BrodaSobolUniformLds UniformLowDiscrepancy;

        // default choice for Gaussian low discepancy sequence generator.
//        typedef BoxMullerGaussianRng<UniformLowDiscrepancy> GaussianLowDiscrepancy;
    }
}

#endif
