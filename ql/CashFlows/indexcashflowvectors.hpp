
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

/*! \file indexcashflowvectors.hpp
    \brief Index Cash flow vector builders
*/

#ifndef quantlib_index_cash_flow_vectors_hpp
#define quantlib_index_cash_flow_vectors_hpp

#include <ql/CashFlows/shortindexedcoupon.hpp>
#include <ql/scheduler.hpp>

namespace QuantLib {

    namespace CashFlows {

		template <class IndexedCouponType>
		std::vector<Handle<CashFlow> > 
        IndexedCouponVector(const Schedule& schedule, 
                            const std::vector<double>& nominals,
                            const Handle<Indexes::Xibor>& index,
                            int fixingDays,
                            const std::vector<Spread>& spreads,
                            const DayCounter& dayCounter = DayCounter()) {

            QL_REQUIRE(nominals.size() != 0, "unspecified nominals");

            std::vector<Handle<CashFlow> > leg;
            // first period might be short or long
            Date start = schedule.date(0), end = schedule.date(1);
            Calendar calendar = schedule.calendar();
            RollingConvention rollingConvention = 
                schedule.rollingConvention();
            Date paymentDate = calendar.roll(end,rollingConvention);
            Spread spread;
            if (spreads.size() > 0)
                spread = spreads[0];
            else
                spread = 0.0;
            double nominal = nominals[0];
            if (schedule.isRegular(1)) {
                leg.push_back(Handle<CashFlow>(
                    new IndexedCouponType(nominal, paymentDate, index, 
                                          start, end, fixingDays, spread, 
                                          start, end, dayCounter)));
            } else {
                Date reference = end.plusMonths(-12/schedule.frequency());
                reference = calendar.roll(reference,rollingConvention);
				typedef Short<IndexedCouponType> ShortIndexedCouponType;
                leg.push_back(Handle<CashFlow>(
                    new ShortIndexedCouponType(nominal, paymentDate, index, 
                                               start, end, fixingDays, spread, 
                                               reference, end, dayCounter)));
            }
            // regular periods
            for (Size i=2; i<schedule.size()-1; i++) {
                start = end; end = schedule.date(i);
                paymentDate = calendar.roll(end,rollingConvention);
                if ((i-1) < spreads.size())
                    spread = spreads[i-1];
                else if (spreads.size() > 0)
                    spread = spreads.back();
                else
                    spread = 0.0;
                if ((i-1) < nominals.size())
                    nominal = nominals[i-1];
                else
                    nominal = nominals.back();
                leg.push_back(Handle<CashFlow>(
                    new IndexedCouponType(nominal, paymentDate, index, 
                                          start, end, fixingDays, spread, 
                                          start, end, dayCounter)));
            }
            if (schedule.size() > 2) {
                // last period might be short or long
                Size N = schedule.size();
                start = end; end = schedule.date(N-1);
                paymentDate = calendar.roll(end,rollingConvention);
                if ((N-2) < spreads.size())
                    spread = spreads[N-2];
                else if (spreads.size() > 0)
                    spread = spreads.back();
                else
                    spread = 0.0;
                if ((N-2) < nominals.size())
                    nominal = nominals[N-2];
                else
                    nominal = nominals.back();
                if (schedule.isRegular(N-1)) {
                    leg.push_back(Handle<CashFlow>(
                        new IndexedCouponType(nominal, paymentDate, index, 
                                              start, end, fixingDays, spread, 
                                              start, end, dayCounter)));
                } else {
                    Date reference = 
                        start.plusMonths(12/schedule.frequency());
                    reference = calendar.roll(reference,rollingConvention);
					typedef Short<IndexedCouponType> ShortIndexedCouponType;
                    leg.push_back(Handle<CashFlow>(
                        new ShortIndexedCouponType(nominal, paymentDate, index,
                                                   start, end, fixingDays, 
                                                   spread, start, reference,
                                                   dayCounter)));
                }
            }
            return leg;
		}

    }

}


#endif
