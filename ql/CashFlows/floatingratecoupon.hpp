
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

/*! \file floatingratecoupon.hpp
    \brief Coupon at par on a term structure
*/

#ifndef quantlib_floating_rate_coupon_hpp
#define quantlib_floating_rate_coupon_hpp

#include <ql/CashFlows/coupon.hpp>

namespace QuantLib {

    //! %coupon at par on a term structure
    /*! \warning This class does not perform any date adjustment,
      i.e., the start and end date passed upon construction
      should be already rolled to a business day.
    */
    class FloatingRateCoupon : public Coupon {
      public:
        FloatingRateCoupon(double nominal, const Date& paymentDate,
                           const Date& startDate, const Date& endDate,
                           int fixingDays, Spread spread = 0.0,
                           const Date& refPeriodStart = Date(),
                           const Date& refPeriodEnd = Date());
        //! \name Coupon interface
        //@{
        double accruedAmount(const Date&) const;
        //@}
        //! \name Inspectors
        //@{
        int fixingDays() const;
        virtual Spread spread() const;
        virtual Rate fixing() const = 0;
        virtual Date fixingDate() const = 0;
        //@}
        //! \name Visitability
        //@{
        virtual void accept(AcyclicVisitor&);
        //@}
      protected:
        int fixingDays_;
        Spread spread_;
    };


    // inline definitions

    inline FloatingRateCoupon::FloatingRateCoupon(
        double nominal, const Date& paymentDate,
        const Date& startDate, const Date& endDate,
        int fixingDays, Spread spread,
        const Date& refPeriodStart, const Date& refPeriodEnd)
    : Coupon(nominal, paymentDate, 
             startDate, endDate, refPeriodStart, refPeriodEnd),
      fixingDays_(fixingDays), spread_(spread) {}

    inline int FloatingRateCoupon::fixingDays() const {
        return fixingDays_;
    }

    inline Spread FloatingRateCoupon::spread() const {
        return spread_;
    }

    inline double FloatingRateCoupon::accruedAmount(const Date& d) const {
        if (d <= accrualStartDate_ || d > paymentDate_) {
            return 0.0;
        } else {
            return nominal()*fixing()*
                dayCounter().yearFraction(accrualStartDate_,
                                          QL_MIN(d,accrualEndDate_),
                                          refPeriodStart_,
                                          refPeriodEnd_);
        }
    }

    inline void FloatingRateCoupon::accept(AcyclicVisitor& v) {
        Visitor<FloatingRateCoupon>* v1 = 
            dynamic_cast<Visitor<FloatingRateCoupon>*>(&v);
        if (v1 != 0)
            v1->visit(*this);
        else
            Coupon::accept(v);
    }

}


#endif
