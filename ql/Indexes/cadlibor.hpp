/*
 Copyright (C) 2000, 2001, 2002 RiskMap srl

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
/*! \file cadlibor.hpp
    \brief %CAD %Libor index (Also known as CDOR, check settlement days)

    \fullpath
    ql/Indexes/%cadlibor.hpp
*/

// $Id$

#ifndef quantlib_cad_libor_hpp
#define quantlib_cad_libor_hpp

#include <ql/Indexes/xibor.hpp>
#include <ql/Calendars/toronto.hpp>
#include <ql/DayCounters/actual365.hpp>

namespace QuantLib {

    namespace Indexes {

        //! %CAD %Libor index (Also known as CDOR, check settlement days)
        class CADLibor : public Xibor {
          public:
            CADLibor(int n, TimeUnit units,
                const RelinkableHandle<TermStructure>& h)
            : Xibor("CADLibor", n, units, 0, CAD,
                Calendar(Calendars::Toronto()), true, ModifiedFollowing,
                DayCounter(DayCounters::Actual365()), h) {}
        };

    }

}


#endif
