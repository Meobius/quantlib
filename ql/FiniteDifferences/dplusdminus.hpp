
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

/*! \file dplusdminus.hpp
    \brief \f$ D_{+}D_{-} \f$ matricial representation
*/

#ifndef quantlib_d_plus_d_minus_h
#define quantlib_d_plus_d_minus_h

#include <ql/FiniteDifferences/tridiagonaloperator.hpp>

namespace QuantLib {

    namespace FiniteDifferences {

        //! \f$ D_{+}D_{-} \f$ matricial representation
        /*! The differential operator \f$  D_{+}D_{-} \f$ discretizes the
            second derivative with the second-order formula
            \f[ \frac{\partial^2 u_{i}}{\partial x^2} \approx
                \frac{u_{i+1}-2u_{i}+u_{i-1}}{h^2} = D_{+}D_{-} u_{i}
            \f]
        */
        class DPlusDMinus : public TridiagonalOperator {
          public:
            DPlusDMinus(Size gridPoints, double h);
        };

        // inline definitions

        inline DPlusDMinus::DPlusDMinus(Size gridPoints, double h)
        : TridiagonalOperator(gridPoints) {
            setFirstRow(0.0,0.0);                   // linear extrapolation
            setMidRows(1/(h*h),-2/(h*h),1/(h*h));
            setLastRow(0.0,0.0);                    // linear extrapolation
        }

    }

}


#endif
