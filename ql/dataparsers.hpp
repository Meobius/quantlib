
/*
 Copyright (C) 2000, 2001, 2002 Andre Louw

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
/*! \file dataparsers.hpp
    \brief Classes used to parse data for input

    \fullpath
    ql/%dataparsers.hpp
*/

// $Id$

#ifndef quantlib_data_parsers_h
#define quantlib_data_parsers_h

#include <ql/date.hpp>

namespace QuantLib {

   class PeriodParser {
   public:
      static Period parse(const std::string str);
   };
}

#endif
