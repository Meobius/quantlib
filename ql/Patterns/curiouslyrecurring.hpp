
/*
 Copyright (C) 2003 RiskMap srl

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

/*! \file curiouslyrecurring.hpp
    \brief Curiously recurring template pattern
*/

#ifndef quantlib_curiously_recurring_h
#define quantlib_curiously_recurring_h

#include <ql/qldefines.hpp>

namespace QuantLib {

    namespace Patterns {

        //! Support for the curiously recurring template pattern
        /*! See James O. Coplien. A Curiously Recurring Template Pattern. 
            In Stanley B. Lippman, editor, C++ Gems, 135-144. 
            Cambridge University Press, New York, New York, 1996.
        */
        template <class Impl>
        class CuriouslyRecurringTemplate {
          protected:
            // not meant to be instantiated as such
            CuriouslyRecurringTemplate() {}
            ~CuriouslyRecurringTemplate() {}
            // support for the curiously recurring template pattern
            Impl& impl() { 
                return static_cast<Impl&>(*this); 
            }
            const Impl& impl() const { 
                return static_cast<const Impl&>(*this); 
            }
        };

    }

}


#endif
