
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

/*! \file indexedcoupon.hpp
    \brief indexed coupon
*/

#ifndef quantlib_indexed_coupon_hpp
#define quantlib_indexed_coupon_hpp

#include <ql/CashFlows/floatingratecoupon.hpp>
#include <ql/Indexes/xibor.hpp>

namespace QuantLib {

    namespace CashFlows {

        //! %base indexed coupon class
        /*! \warning This class does not perform any date adjustment,
            i.e., the start and end date passed upon construction
            should be already rolled to a business day.
        */
        class IndexedCoupon : public FloatingRateCoupon,
                              public Patterns::Observer {
          public:
            IndexedCoupon(double nominal,
                          const Date& paymentDate,
                          const Handle<Indexes::Xibor>& index,
                          const Date& startDate, const Date& endDate,
                          int fixingDays,
                          Spread spread = 0.0,
                          const Date& refPeriodStart = Date(),
                          const Date& refPeriodEnd = Date(),
                          const DayCounter& dayCounter = DayCounter());
  			virtual ~IndexedCoupon() {}
            //! \name CashFlow interface
            //@{
            double amount() const;
            //@}
            //! \name Coupon interface
            //@{
            DayCounter dayCounter() const;
            //@}
			//! \name FloatingRateCoupon interface
            //@{
            Rate fixing() const;
            //@}
            //! \name Inspectors
            //@{
            const Handle<Indexes::Xibor>& index() const;
            //@}
            //! \name Observer interface
            //@{
            void update();
            //@}
            //! \name Visitability
            //@{
            virtual void accept(Patterns::AcyclicVisitor&);
            //@}
          private:
            Handle<Indexes::Xibor> index_;
            DayCounter dayCounter_;
        };


        // inline definitions
        inline IndexedCoupon::IndexedCoupon(
                double nominal,
                const Date& paymentDate,
                const Handle<Indexes::Xibor>& index,
                const Date& startDate, const Date& endDate,
                int fixingDays, Spread spread,
                const Date& refPeriodStart, const Date& refPeriodEnd,
                const DayCounter& dayCounter)
        : FloatingRateCoupon(nominal, paymentDate, startDate, endDate,
							 fixingDays, spread, refPeriodStart, refPeriodEnd),
          index_(index), dayCounter_(dayCounter) {
            if (dayCounter_.isNull())
                dayCounter_ = index_->dayCounter();
            registerWith(index_);
        }


        inline const Handle<Indexes::Xibor>&
        IndexedCoupon::index() const {
            return index_;
        }

        inline Rate IndexedCoupon::fixing() const {
            return index_->fixing(fixingDate()) + spread();
        }

        inline DayCounter IndexedCoupon::dayCounter() const {
            return dayCounter_;
        }

		inline double IndexedCoupon::amount() const {
			return fixing() * accrualPeriod() * nominal();
		}

        inline void IndexedCoupon::update() {
            notifyObservers();
        }

        inline void IndexedCoupon::accept(Patterns::AcyclicVisitor& v) {
            using namespace Patterns;
            Visitor<IndexedCoupon>* v1 =
                dynamic_cast<Visitor<IndexedCoupon>*>(&v);
            if (v1 != 0)
                v1->visit(*this);
            else
                FloatingRateCoupon::accept(v);
        }

    }

}


#endif
