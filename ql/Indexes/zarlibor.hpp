
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

/*! \file zarlibor.hpp
    \brief %ZAR %Libor index (also known as JIBAR)
*/

#ifndef quantlib_zar_libor_hpp
#define quantlib_zar_libor_hpp

#include <ql/Indexes/xibor.hpp>
#include <ql/Calendars/johannesburg.hpp>
#include <ql/DayCounters/actual365.hpp>

namespace QuantLib {

    namespace Indexes {

        //! %ZAR %Libor index (also known as JIBAR)
        /*! \todo check settlement days */
        class ZARLibor : public Xibor {
          public:
            ZARLibor(int n, TimeUnit units,
                     const RelinkableHandle<TermStructure>& h,
                     const DayCounter& dc = Actual365())
            : Xibor("ZARLibor", n, units, 0, ZAR,
                    Johannesburg(), true, ModifiedFollowing, dc, h) {}
        };

    }

}


#endif
