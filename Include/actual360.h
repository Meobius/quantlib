
/*
 * Copyright (C) 2000
 * Ferdinando Ametrano, Luigi Ballabio, Adolfo Benin, Marco Marchioro
 * 
 * This file is part of QuantLib
 * QuantLib is a C++ open source library for financial quantitative
 * analysts and developers --- http://quantlib.sourceforge.net/
 *
 * QuantLib is free software and you are allowed to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of it under the conditions stated 
 * in the QuantLib License: see the file LICENSE.TXT for details.
 * Contact ferdinando@ametrano.net if LICENSE.TXT was not distributed with this file.
 * LICENCE.TXT is also available at http://quantlib.sourceforge.net/LICENSE.TXT
*/


#ifndef quantlib_actual360_day_counter_h
#define quantlib_actual360_day_counter_h

#include "qldefines.h"
#include "daycounter.h"

namespace QuantLib {

	namespace DayCounters {
	
		class Actual360 : public DayCounter {
		  public:
			std::string name() const { return std::string("act/360"); }
			int dayCount(const Date& d1, const Date& d2) const { return (d2-d1); }
			Time yearFraction(const Date& d1, const Date& d2, 
			  const Date& refPeriodStart = Date(), const Date& refPeriodEnd = Date()) const {
				return dayCount(d1,d2)/360.0;
			}
		};
	
	}

}


#endif
