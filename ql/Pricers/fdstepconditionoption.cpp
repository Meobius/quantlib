

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
/*! \file stepconditionoption.cpp
    \brief Option requiring additional code to be executed at each time step

    \fullpath
    Pricers/%stepconditionoption.cpp
*/

// $Id$

#include <ql/Pricers/fdstepconditionoption.hpp>
#include <ql/Pricers/europeanoption.hpp>
#include <ql/FiniteDifferences/valueatcenter.hpp>

namespace QuantLib {

    namespace Pricers {

        using FiniteDifferences::StandardStepCondition;
        using FiniteDifferences::StandardFiniteDifferenceModel;
        using FiniteDifferences::valueAtCenter;
        using FiniteDifferences::firstDerivativeAtCenter;
        using FiniteDifferences::secondDerivativeAtCenter;

        FdStepConditionOption::FdStepConditionOption(Option::Type type,
            double underlying, double strike, Spread dividendYield,
            Rate riskFreeRate, Time residualTime, double volatility,
            int timeSteps, int gridPoints)
        : FdBsmOption(type, underlying, strike, dividendYield,
            riskFreeRate, residualTime, volatility, gridPoints),
            timeSteps_(timeSteps) {}

        void FdStepConditionOption::calculate() const {

            setGridLimits(underlying_, residualTime_);
            initializeGrid();
            initializeInitialCondition();
            initializeOperator();
            initializeStepCondition();
            /* StandardFiniteDifferenceModel is Crank-Nicolson.
               Alternatively, ImplicitEuler or ExplicitEuler
               could have been used instead*/
            StandardFiniteDifferenceModel model(finiteDifferenceOperator_);

            // Control-variate variance reduction:
            // 1) calculate value/greeks of the European option analytically
            EuropeanOption analyticEuro(type_, underlying_, strike_,
                dividendYield_, riskFreeRate_, residualTime_, volatility_);

            // 2) Initialize prices on the grid
            Array europeanPrices = initialPrices_;
            Array americanPrices = initialPrices_;

            // 3) Rollback
            model.rollback(europeanPrices, residualTime_, 0.0, timeSteps_);
            model.rollback(americanPrices, residualTime_, 0.0, timeSteps_,
                           stepCondition_);

            /* 4) Numerically calculate option value and greeks using
                  the european option as control variate                */

            value_ =  valueAtCenter(americanPrices)
                    - valueAtCenter(europeanPrices)
                    + analyticEuro.value();

            delta_ =   firstDerivativeAtCenter(americanPrices, grid_)
                     - firstDerivativeAtCenter(europeanPrices, grid_)
                     + analyticEuro.delta();

            gamma_ =   secondDerivativeAtCenter(americanPrices, grid_)
                     - secondDerivativeAtCenter(europeanPrices, grid_)
                     + analyticEuro.gamma();

            hasBeenCalculated_ = true;
        }

    }

}
