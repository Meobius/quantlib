
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

/*! \file blackkarasinski.hpp
    \brief Black-Karasinski model
*/

#ifndef quantlib_one_factor_models_black_karasinski_h
#define quantlib_one_factor_models_black_karasinski_h

#include <ql/ShortRateModels/onefactormodel.hpp>

namespace QuantLib {

    namespace ShortRateModels {

        //! Standard Black-Karasinski model class.
        /*! This class implements the standard Black-Karasinski model defined by
            \f[ 
                d\ln r_t = (\theta(t) - \alpha \ln r_t)dt + \sigma dW_t,
            \f]
            where \f$ alpha \f$ and \f$ sigma \f$ are constants.
        */
        class BlackKarasinski : public OneFactorModel,
                                public TermStructureConsistentModel {
          public:
            BlackKarasinski(
                const RelinkableHandle<TermStructure>& termStructure,
                double a = 0.1, double sigma = 0.1);

            Handle<ShortRateDynamics> dynamics() const {
                throw Error("No defined process for Black-Karasinski");
            }

            Handle<Lattices::Lattice> tree(const TimeGrid& grid) const;

          private:
            class Dynamics;
            class Helper;

            double a() const { return a_(0.0); }
            double sigma() const { return sigma_(0.0); }

            Parameter& a_;
            Parameter& sigma_;
        };

        //! Short-rate dynamics in the Black-Karasinski model
        /*! The short-rate is here
            \f[
                r_t = e^{\varphi(t) + x_t}
             \f]
             where \f$ \varphi(t) \f$ is the deterministic time-dependent 
             parameter (which can not be determined analytically)
             used for term-structure fitting and \f$ x_t \f$ is the state
             variable following an Ornstein-Uhlenbeck process.
        */
        class BlackKarasinski::Dynamics 
        : public BlackKarasinski::ShortRateDynamics {
          public:
            Dynamics(const Parameter& fitting, double alpha, double sigma)
            : ShortRateDynamics(Handle<DiffusionProcess>(new 
                  OrnsteinUhlenbeckProcess(alpha, sigma))),
              fitting_(fitting) {}

            double variable(Time t, Rate r) const {
                return QL_LOG(r) - fitting_(t);
            }

            double shortRate(Time t, double x) const {
                return QL_EXP(x + fitting_(t));
            }
          private:
            Parameter fitting_;
        };

    }

}

#endif
