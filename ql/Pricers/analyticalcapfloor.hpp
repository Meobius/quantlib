/*
 Copyright (C) 2001, 2002 Sadruddin Rejeb

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
/*! \file analyticalcapfloor.hpp
    \brief Analytical pricer for caps/floors

    \fullpath
    ql/Pricers/%analyticalcapfloor.hpp
*/

// $Id$

#ifndef quantlib_pricers_analytical_cap_floor_h
#define quantlib_pricers_analytical_cap_floor_h

#include <ql/Instruments/capfloor.hpp>

namespace QuantLib {

    namespace Pricers {

        //! Analytical pricer for cap/floor
        class AnalyticalCapFloor 
        : public CapFloorPricingEngine<InterestRateModelling::AffineModel> {
          public:
            AnalyticalCapFloor(
                const Handle<InterestRateModelling::AffineModel>& model) 
            : CapFloorPricingEngine<InterestRateModelling::AffineModel>(model) 
            {}
            void calculate() const;
        };

    }

}

#endif
