
/*
 Copyright (C) 2003 Nicolas Di C�sar�

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

/*! \file inarrearindexedcoupon.hpp
    \brief in arrear indexed coupon
*/

#ifndef quantlib_in_arrear_indexed_coupon_hpp
#define quantlib_in_arrear_indexed_coupon_hpp

#include <ql/CashFlows/indexedcoupon.hpp>

namespace QuantLib {

    //! %in arrear indexed coupon class
    /*! \warning This class does not perform any date adjustment,
                 i.e., the start and end date passed upon construction
                 should be already rolled to a business day.
    */
    class InArrearIndexedCoupon : public IndexedCoupon {
      public:
        InArrearIndexedCoupon(double nominal,
                              const Date& paymentDate,
                              const Handle<Indexes::Xibor>& index,
                              const Date& startDate, const Date& endDate,
                              int fixingDays,
                              Spread spread = 0.0,
                              const Date& refPeriodStart = Date(),
                              const Date& refPeriodEnd = Date(),
                              const DayCounter& dayCounter = DayCounter())
        : IndexedCoupon(nominal, paymentDate, index, startDate, endDate,
                        fixingDays, spread, refPeriodStart, refPeriodEnd, 
                        dayCounter) {}
        //! \name FloatingRateCoupon interface
        //@{
        Date fixingDate() const;
        //@}
        //! \name Visitability
        //@{
        virtual void accept(Patterns::AcyclicVisitor&);
        //@}
    };


    // inline definitions

    inline Date InArrearIndexedCoupon::fixingDate() const {
        // fix at the end of period
        return index()->calendar().advance(accrualEndDate_, 
                                           -fixingDays_, Days,
                                           Preceding);
    }

    inline 
    void InArrearIndexedCoupon::accept(Patterns::AcyclicVisitor& v) {
        using namespace Patterns;
        Visitor<InArrearIndexedCoupon>* v1 =
            dynamic_cast<Visitor<InArrearIndexedCoupon>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            IndexedCoupon::accept(v);
    }

}


#endif
