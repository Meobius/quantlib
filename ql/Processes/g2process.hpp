/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Banca Profilo S.p.A.

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file g2process.hpp
    \brief G2 stochastic processes
*/

#ifndef quantlib_g2_process_hpp
#define quantlib_g2_process_hpp

#include <ql/Processes/forwardmeasureprocess.hpp>
#include <ql/Processes/ornsteinuhlenbeckprocess.hpp>

namespace QuantLib {

    //! G2 stochastic process
    class G2Process : public StochasticProcess {
      public:
        G2Process(Real a, Real sigma, Real b, Real eta, Real rho);
        //! \name StochasticProcess interface
        //@{
        Size size() const;
        Disposable<Array> initialValues() const;
        Disposable<Array> drift(Time t, const Array& x) const;
        Disposable<Matrix> diffusion(Time t, const Array& x) const;
        Disposable<Array> expectation(Time t0, const Array& x0, Time dt) const;
        Disposable<Matrix> stdDeviation(Time t0, const Array& x0,
                                        Time dt) const;
        Disposable<Matrix> covariance(Time t0, const Array& x0, Time dt) const;
        //@}
        Real x0() const;
        Real y0() const;
        Real a() const;
        Real sigma() const;
        Real b() const;
        Real eta() const;
        Real rho() const;
      private:
        Real x0_, y0_, a_, sigma_, b_, eta_, rho_;
        boost::shared_ptr<QuantLib::OrnsteinUhlenbeckProcess> xProcess_;
        boost::shared_ptr<QuantLib::OrnsteinUhlenbeckProcess> yProcess_;
    };

    //! forward G2 stochastic process
    class G2ForwardProcess : public ForwardMeasureProcess {
      public:
        G2ForwardProcess(Real a, Real sigma, Real b, Real eta, Real rho);
        //! \name StochasticProcess interface
        //@{
        Size size() const;
        Disposable<Array> initialValues() const;
        Disposable<Array> drift(Time t, const Array& x) const;
        Disposable<Matrix> diffusion(Time t, const Array& x) const;
        Disposable<Array> expectation(Time t0, const Array& x0, Time dt) const;
        Disposable<Matrix> stdDeviation(Time t0, const Array& x0,
                                        Time dt) const;
        Disposable<Matrix> covariance(Time t0, const Array& x0, Time dt) const;
        //@}
      protected:
        Real x0_, y0_, a_, sigma_, b_, eta_, rho_;
        boost::shared_ptr<QuantLib::OrnsteinUhlenbeckProcess> xProcess_;
        boost::shared_ptr<QuantLib::OrnsteinUhlenbeckProcess> yProcess_;
        Real xForwardDrift(Time t, Time T) const;
        Real yForwardDrift(Time t, Time T) const;
        Real Mx_T(Real s, Real t, Real T) const;
        Real My_T(Real s, Real t, Real T) const;
    };

}


#endif

