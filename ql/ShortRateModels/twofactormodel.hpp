
/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file twofactormodel.hpp
    \brief Abstract two-factor interest rate model class
*/

#ifndef quantlib_interest_rate_modelling_two_factor_model_h
#define quantlib_interest_rate_modelling_two_factor_model_h

#include <ql/diffusionprocess.hpp>
#include <ql/ShortRateModels/model.hpp>
#include <ql/Lattices/lattice2d.hpp>

namespace QuantLib {

    //! Abstract base-class for two-factor models
    class TwoFactorModel : public ShortRateModel {
      public:
        TwoFactorModel(Size nParams);

        class ShortRateDynamics;
        class ShortRateTree;

        //! Returns the short-rate dynamics
        virtual Handle<ShortRateDynamics> dynamics() const = 0;

        //! Returns a two-dimensional trinomial tree
        virtual Handle<Lattice> tree(const TimeGrid& grid) const;

    };

    //! Class describing the dynamics of the two state variables
    /*! We assume here that the short-rate is a function of two state
        variables x and y.
        \f[
            r_t = f(t, x_t, y_t)
        \f]
        of two state variables \f$ x_t \f$ and \f$ y_t \f$. These stochastic
        processes satisfy
        \f[
            x_t = \mu_x(t, x_t)dt + \sigma_x(t, x_t) dW_t^x
        \f]
        and
        \f[
            y_t = \mu_y(t,y_t)dt + \sigma_y(t, y_t) dW_t^y
        \f]
        where \f$ W^x \f$ and \f$ W^y \f$ are two brownian motions 
        satisfying
        \f[
            dW^x_t dW^y_t = \rho dt
        \f].
    */
    class TwoFactorModel::ShortRateDynamics {
      public:
        ShortRateDynamics(const Handle<DiffusionProcess>& xProcess,
                          const Handle<DiffusionProcess>& yProcess,
                          double correlation)
        : xProcess_(xProcess), yProcess_(yProcess), 
          correlation_(correlation) {}
        virtual ~ShortRateDynamics() {}

        virtual Rate shortRate(Time t, double x, double y) const = 0;

        //! Risk-neutral dynamics of the first state variable x
        const Handle<DiffusionProcess>& xProcess() const {
            return xProcess_;
        }

        //! Risk-neutral dynamics of the second state variable y
        const Handle<DiffusionProcess>& yProcess() const {
            return yProcess_;
        }

        //! Correlation \f$ \rho \f$ between the two brownian motions.
        double correlation() const {
            return correlation_;
        }
      private:
        Handle<DiffusionProcess> xProcess_, yProcess_;
        double correlation_;
    };

    //! Recombining two-dimensional tree discretizing the state variable
    class TwoFactorModel::ShortRateTree : public Lattice2D {
      public:
        //! Plain tree build-up from short-rate dynamics
        ShortRateTree(
                      const Handle<TrinomialTree>& tree1,
                      const Handle<TrinomialTree>& tree2,
                      const Handle<ShortRateDynamics>& dynamics);

        DiscountFactor discount(Size i, Size index) const {
            Size modulo = tree1_->size(i);
            Size index1 = index % modulo;
            Size index2 = index / modulo;

            double x = tree1_->underlying(i, index1);
            double y = tree1_->underlying(i, index2);

            Rate r = dynamics_->shortRate(timeGrid()[i], x, y);
            return QL_EXP(-r*timeGrid().dt(i));
        }
      private:
        Handle<ShortRateDynamics> dynamics_;
    };

}


#endif
