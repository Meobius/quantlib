
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

// $Id$

#ifndef quantlib_config_bcc_h
#define quantlib_config_bcc_h

/*************************************************************
   User configuration section:
   modify the following definitions to suit your preferences.
 *************************************************************/

/* Define this if error messages should include file and line information. */
// #define QL_ERROR_LINES

/* Define this if payments occurring today should not enter the NPV of an
   instrument. */
// #define QL_EARLY_PAYMENTS

/* Define this if payments occurring today should not enter the NPV of a swap.
   */
// #define QL_EARLY_SWAP_PAYMENTS

/* Define this if payments occurring today should enter the NPV of an
   instrument. */
// #define QL_LATE_PAYMENTS

/* Define this if payments occurring today should enter the NPV of a swap. */
// #define QL_LATE_SWAP_PAYMENTS


/*******************************************
   System configuration section:
   do not modify the following definitions.
 *******************************************/

#define HAVE_CSTDLIB 1
#define QL_SIZE_T    std::size_t
#define QL_ATOI      std::atoi

#define HAVE_CMATH   1
#define QL_SQRT      std::sqrt
#define QL_FABS      std::fabs
#define QL_EXP       std::exp
#define QL_LOG       std::log
#define QL_SIN       std::sin
#define QL_COS       std::cos
#define QL_POW       std::pow
#define QL_MODF      std::modf
#define QL_FLOOR     std::floor

/* Useful constants missing in Borland math.h  */
#define M_IVLN10    0.434294481903251827651 /* 1 / log(10) */
#define M_LOG2_E    0.693147180559945309417
#define M_TWOPI     (M_PI * 2.0)
#define M_3PI_4     2.3561944901923448370E0
#define M_SQRTPI    1.77245385090551602792981
#define M_SQRT1_2   0.7071067811865475244008443621048490392848359376887
#define M_LN2LO     1.9082149292705877000E-10
#define M_LN2HI     6.9314718036912381649E-1
#define M_SQRT3     1.73205080756887719000
#define M_INVLN2    1.4426950408889633870E0  /* 1 / log(2) */

#define HAVE_LIMITS  1

#define HAVE_CTIME   1
#define QL_TIME_T    std::time_t
#define QL_TM        std::tm
#define QL_CLOCK     std::clock
#define QL_TIME      std::time
#define QL_GMTIME    std::gmtime

#define HAVE_CCTYPE  1
#define QL_TOLOWER   std::tolower
#define QL_TOUPPER   std::toupper

#define HAVE_CSTRING 1
#define QL_STRLEN    std::strlen

#define HAVE_CSTDIO  1
#define QL_SPRINTF   std::sprintf

#define QL_MIN       std::min
#define QL_MAX       std::max

#define HAVE_EXPRESSION_TEMPLATES

#define QL_ITERATOR         std::iterator
#define QL_ITERATOR_TRAITS  std::iterator_traits

#ifdef DEBUG
    #define QL_DEBUG
#endif

#endif
