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
/*! \file coxingersollross.hpp
    \brief Cox-Ingersoll-Ross model

    \fullpath
    ql/ShortRateModels/OneFactorModels/%coxingersollross.hpp
*/

// $Id$

#ifndef quantlib_one_factor_models_cox_ingersoll_ross_h
#define quantlib_one_factor_models_cox_ingersoll_ross_h

#include <ql/ShortRateModels/onefactormodel.hpp>

namespace QuantLib {

    namespace ShortRateModels {

        //! Cox-Ingersoll-Ross model class.
        /*! This class implements the Cox-Ingersoll-Ross model defined by 
            \f[ 
                dr_t = (\theta - \alpha r_t)dt + \sqrt{r_t}\sigma dW_t .
            \f]
            It is actually implemented as \f$ r_t = y_t^2 \f$
            where y_t is defined by 
            \f[
                dy_t=\left[ 
                        (\frac{k\theta }{2}+\frac{\sigma ^2}{8})\frac{1}{y_t}-
                        \frac{k}{2}y_t \right] d_t+ \frac{\sigma }{2}dW_{t}
            \f]
        */
        class CoxIngersollRoss : public OneFactorModel, 
                                 public OneFactorAffineModel {
          public:
            CoxIngersollRoss(double theta = 0.1, double k = 0.1, 
                             double sigma = 0.1, double r0 = 0.05);

            virtual double discountBond(Time t, Time T, Rate r) const;
            virtual double discountBondOption(Option::Type type,
                                              double strike,
                                              Time maturity,
                                              Time bondMaturity) const;

            virtual Handle<ShortRateDynamics> dynamics() const;

            virtual Handle<Lattices::Tree> tree(const TimeGrid& grid) const {
                return Handle<Lattices::Tree>(
                    new ShortRateTree(dynamics(), grid, true));
            }

            class Dynamics;
          protected:
            double A(Time t) const;
            double B(Time t) const;

            double theta() const { return theta_(0.0); }
            double k() const { return k_(0.0); }
            double sigma() const { return sigma_(0.0); }
            double x0() const { return r0_(0.0); }
            
          private:
            class VolatilityConstraint;

            Parameter& theta_;
            Parameter& k_;
            Parameter& sigma_;
            Parameter& r0_;
        };

        class SquareRootProcess : public DiffusionProcess {
          public:
            SquareRootProcess(double theta, double k, double sigma, double y0) 
            : DiffusionProcess(y0), theta_(theta), k_(k), sigma_(sigma) {}

            double drift(Time t, double y) const {
                return (0.5*theta_*k_ - 0.125*sigma_*sigma_)/y 
                       - 0.5*k_*y;
            }
            double diffusion(Time t, double y) const {
                return 0.5*sigma_;
            }

          private:
            double theta_, k_, sigma_;
        };

        class CoxIngersollRoss::Dynamics : public ShortRateDynamics {
          public:
            Dynamics(double theta,
                     double k,
                     double sigma,
                     double x0)
            : ShortRateDynamics(Handle<DiffusionProcess>(
                  new SquareRootProcess(theta, k, sigma, QL_SQRT(x0)))) {}

            virtual double variable(Time t, Rate r) const {
                return QL_SQRT(r);
            }
            virtual double shortRate(Time t, double y) const {
                return y*y;
            }
        };
    
    }

}

#endif
