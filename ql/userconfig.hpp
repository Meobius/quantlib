
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

#ifndef quantlib_config_h
#define quantlib_config_h

/***************************************************************
   User configuration section:
   modify the following definitions to suit your preferences.

   Do not modify this file if you are using a Linux/Unix system:
   it will not be read by the compiler. The definitions below 
   will be provided by running ./configure instead.
****************************************************************/

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


#endif
