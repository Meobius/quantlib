
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

#ifndef quantlib_config_msvc_h
#define quantlib_config_msvc_h

/*************************************************************
   User configuration section:
   modify the following definitions to suit your preferences.
 *************************************************************/

/* Define this if the Boost library is available. */
// #define HAVE_BOOST 1

/* Define this if error messages should include file and line information. */
// #define QL_ERROR_LINES 1

/* Define this if negative yield rates should be allowed. This might not be
   safe. */
// #define QL_NEGATIVE_RATES 1

/* Define this if extra safety checks should be performed. This can degrade
   performance. */
// #define QL_EXTRA_SAFETY_CHECKS 1

/* Define this if payments occurring today should enter the NPV of an
   instrument. */
// #define QL_TODAYS_PAYMENTS 1


/*******************************************
   System configuration section:
   do not modify the following definitions.
 *******************************************/

// disable useless warning
// 'identifier' : decorated name length exceeded,
//                name was truncated in debug info
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)

#if _MSC_VER == 1200
// move inside here configs specific to VC++ 6.0
#define GARBLED_REVERSE_ITERATOR
#define CHOKES_ON_TYPENAME


#elif _MSC_VER == 1300
// move inside here configs specific to VC++ .Net
#define QL_ITERATOR         std::iterator
#define QL_ITERATOR_TRAITS  std::iterator_traits
#define SPECIALIZE_MS_ITERATOR_TRAITS  // always new ways to surprise us...


#endif


// STLPort or VC++ implementation?
#if defined(__SGI_STL)
    #define QL_MIN   _STL::min
    #define QL_MAX   _STL::max
#else
    #define QL_MIN   std::_cpp_min
    #define QL_MAX   std::_cpp_max
#endif


// leave outside here common configs


// force undefining min and max macros
#ifndef NOMINMAX
    #error Add NOMINMAX to preprocessor definitions
#endif

#define REQUIRES_DUMMY_RETURN

#define HAVE_CSTDLIB 1
#define QL_SIZE_T    size_t
#define QL_ATOI      atoi

#define HAVE_CMATH   1
#define QL_SQRT      sqrt
#define QL_FABS      fabs
#define QL_EXP       exp
#define QL_LOG       log
#define QL_SIN       sin
#define QL_COS       cos
#define QL_POW       pow
#define QL_MODF      modf
#define QL_FLOOR     floor

/* Useful constants missing in Visual C++ math.h  */
#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_IVLN10    0.434294481903251827651 /* 1 / log(10) */
#define M_LN2       0.693147180559945309417
#define M_LOG2_E    0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI        3.141592653589793238462643383280
#define M_TWOPI     (M_PI * 2.0)
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#define M_3PI_4     2.3561944901923448370E0
#define M_SQRTPI    1.77245385090551602792981
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.7071067811865475244008443621048490392848359376887
#define M_SQRT1_2   0.7071067811865475244008443621048490392848359376887
#define M_LN2LO     1.9082149292705877000E-10
#define M_LN2HI     6.9314718036912381649E-1
#define M_SQRT3     1.73205080756887719000
#define M_INVLN2    1.4426950408889633870E0  /* 1 / log(2) */



#define HAVE_LIMITS  1

#define HAVE_CTIME   1
#define QL_TIME_T    time_t
#define QL_TM        tm
#define QL_CLOCK     clock
#define QL_TIME      time
#define QL_GMTIME    gmtime

#define HAVE_CCTYPE  1
#define QL_TOLOWER   tolower
#define QL_TOUPPER   toupper

#define HAVE_CSTRING 1
#define QL_STRLEN    strlen

#define HAVE_CSTDIO  1
#define QL_SPRINTF   sprintf

#define BROKEN_TEMPLATE_SPECIALIZATION
#define BROKEN_TEMPLATE_METHOD_CALLS
#define HAVE_TEMPLATE_METAPROGRAMMING

#define HAVE_INCOMPLETE_ITERATOR_SUPPORT

#define QL_PATCH_MICROSOFT

/*** libraries to be linked ***/

#ifdef _DLL
    #ifdef _DEBUG
        #pragma comment(lib,"QuantLib_MTDLL_d.lib")
    #else
        #pragma comment(lib,"QuantLib_MTDLL.lib")
    #endif
#else
    #ifdef _DEBUG
        #pragma comment(lib,"QuantLib_d.lib")
    #else
        #pragma comment(lib,"QuantLib.lib")
    #endif
#endif

#ifndef _CPPRTTI
    #error Check the 'C/C++ | C++ Language | Use RTTI' option
#endif

#endif
