
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

/*! \file fdmultiperiodoption.hpp
    \brief base class for option with events happening at different periods
*/

#ifndef quantlib_multi_period_option_pricer_h
#define quantlib_multi_period_option_pricer_h

#include <ql/Pricers/fdbsmoption.hpp>
#include <ql/FiniteDifferences/fdtypedefs.hpp>

namespace QuantLib {

    class FdMultiPeriodOption : public FdBsmOption {
      public:
        double controlVariateCorrection() const;
      protected:
        // constructor
        FdMultiPeriodOption(Option::Type type, double underlying, 
                            double strike, Spread dividendYield, 
                            Rate riskFreeRate, Time residualTime, 
                            double volatility, int gridPoints,
                            const std::vector<Time>& dates,
                            int timeSteps);
        // Protected attributes
        std::vector<Time> dates_;
        Size dateNumber_;
        int timeStepPerPeriod_;
        bool lastDateIsResTime_;
        int lastIndex_;
        bool firstDateIsZero_;
        double firstNonZeroDate_;
        int firstIndex_;
        mutable Handle<SingleAssetOption> analytic_;
        mutable Array prices_, controlPrices_;
        mutable Handle<StandardStepCondition> stepCondition_;
        mutable Handle<StandardFiniteDifferenceModel> model_;
        // Methods
        void calculate() const;
        virtual void initializeControlVariate() const;
        virtual void initializeModel() const;
        virtual void initializeStepCondition() const;
        virtual void executeIntermediateStep(int step) const = 0;
      private:
        mutable double controlVariateCorrection_;
    };

}


#endif
