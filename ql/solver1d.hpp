
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

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

/*! \file solver1d.hpp
    \brief Abstract 1-D solver class
*/

#ifndef quantlib_solver1d_h
#define quantlib_solver1d_h

#include <ql/null.hpp>
#include <ql/dataformatters.hpp>

namespace QuantLib {

    /*! \namespace QuantLib::Solvers1D
        \brief Implementations of the Solver1D interface

        See sect. \ref solvers1d
    */

    #define MAX_FUNCTION_EVALUATIONS 100

    //! Base class for 1-D solvers
    /*! The implementation of this class uses the so-called
        "Barton-Nackman trick", also known as "the curiously recurring
        template pattern". Concrete solvers will be declared as:
        \code
        class Foo : public Solver1D<Foo> {
          public:
            ...
            template <class F>
            double solveImpl(const F& f, double accuracy) const {
                ...
            }
        };
        \endcode
        Before calling <tt>solveImpl</tt>, the base class will set its
        protected data members so that:
        - <tt>xMin_</tt> and  <tt>xMax_</tt> form a valid bracket;
        - <tt>fxMin_</tt> and <tt>fxMax_</tt> contain the values of
          the function in <tt>xMin_</tt> and <tt>xMax_</tt>;
        - <tt>root_</tt> is a valid initial guess.
        The implementation of <tt>solveImpl</tt> can safely assume all
        of the above.
    */
    template <class Impl>
    class Solver1D {
      public:
        Solver1D()
        : maxEvaluations_(MAX_FUNCTION_EVALUATIONS),
          lowerBoundEnforced_(false), upperBoundEnforced_(false) {}
        //! \name Modifiers
        //@{
        /*! This method returns the zero of the function \f$ f \f$,
            determined with the given accuracy (i.e., \f$ x \f$ is
            considered a zero if \f$ |f(x)| < accuracy \f$).  This
            method contains a bracketing routine to which an initial
            guess must be supplied as well as a step used to scan the
            range of the possible bracketing values.
        */
        template <class F>
        double solve(const F& f,
                     double accuracy,
                     double guess,
                     double step) const {

            const double growthFactor = 1.6;
            int flipflop = -1;

            root_ = guess;
            fxMax_ = f(root_);

            // monotonically crescent bias, as in optionValue(volatility)
            if (QL_FABS(fxMax_) <= accuracy)
                return root_;
            else if (fxMax_ > 0.0) {
                xMin_ = enforceBounds_(root_ - step);
                fxMin_ = f(xMin_);
                xMax_ = root_;
            } else {
                xMin_ = root_;
                fxMin_ = fxMax_;
                xMax_ = enforceBounds_(root_+step);
                fxMax_ = f(xMax_);
            }

            evaluationNumber_ = 2;
            while (evaluationNumber_ <= maxEvaluations_) {
                if (fxMin_*fxMax_ <= 0.0) {
                    if (fxMin_ == 0.0)    return xMin_;
                    if (fxMax_ == 0.0)    return xMax_;
                    root_ = (xMax_+xMin_)/2.0;
                    // check whether we really want to pass epsilon
                    return impl().solveImpl(f, QL_MAX(QL_FABS(accuracy), 
                                                      QL_EPSILON));
                }
                if (QL_FABS(fxMin_) < QL_FABS(fxMax_)) {
                    xMin_ = enforceBounds_(xMin_+growthFactor*(xMin_ - xMax_));
                    fxMin_= f(xMin_);
                } else if (QL_FABS(fxMin_) > QL_FABS(fxMax_)) {
                    xMax_ = enforceBounds_(xMax_+growthFactor*(xMax_ - xMin_));
                    fxMax_= f(xMax_);
                } else if (flipflop == -1) {
                    xMin_ = enforceBounds_(xMin_+growthFactor*(xMin_ - xMax_));
                    fxMin_= f(xMin_);
                    evaluationNumber_++;
                    flipflop = 1;
                } else if (flipflop == 1) {
                    xMax_ = enforceBounds_(xMax_+growthFactor*(xMax_ - xMin_));
                    fxMax_= f(xMax_);
                    flipflop = -1;
                }
                evaluationNumber_++;
            }

            throw Error("unable to bracket root in " +
                        IntegerFormatter::toString(maxEvaluations_) +
                        " function evaluations (last bracket attempt: f[" +
                        DoubleFormatter::toString(xMin_) + "," + 
                        DoubleFormatter::toString(xMax_) + "] -> [" +
                        DoubleFormatter::toExponential(fxMin_) + "," +
                        DoubleFormatter::toExponential(fxMax_) + "])");
        }
        /*! This method returns the zero of the function \f$ f \f$,
            determined with the given accuracy (i.e., \f$ x \f$ is
            considered a zero if \f$ |f(x)| < accuracy \f$). An
            initial guess must be supplied, as well as two values \f$
            x_\mathrm{min} \f$ and \f$ x_\mathrm{max} \f$ which must
            bracket the zero (i.e., either \f$ f(x_\mathrm{min}) \leq
            0 \leq f(x_\mathrm{max}) \f$, or \f$ f(x_\mathrm{max})
            \leq 0 \leq f(x_\mathrm{min}) \f$ must be true).
        */
        template <class F>
        double solve(const F& f,
                     double accuracy,
                     double guess,
                     double xMin,
                     double xMax) const {

            xMin_ = xMin;
            xMax_ = xMax;

            QL_REQUIRE(xMin_ < xMax_, "invalid range: xMin_ (" +
                       DoubleFormatter::toString(xMin_) +
                       ") >= xMax_ (" + 
                       DoubleFormatter::toString(xMax_) + ")");
            QL_REQUIRE(!lowerBoundEnforced_ || xMin_ >= lowerBound_, 
                       "xMin_ (" +
                       DoubleFormatter::toString(xMin_) + 
                       ") < enforced low bound (" +
                       DoubleFormatter::toString(lowerBound_) + ")");
            QL_REQUIRE(!upperBoundEnforced_ || xMax_ <= upperBound_, 
                       "xMax_ (" +
                       DoubleFormatter::toString(xMax_) +
                       ") > enforced hi bound (" +
                       DoubleFormatter::toString(upperBound_) + ")");

            fxMin_ = f(xMin_);
            if (QL_FABS(fxMin_) < accuracy)
                return xMin_;

            fxMax_ = f(xMax_);
            if (QL_FABS(fxMax_) < accuracy)
                return xMax_;

            evaluationNumber_ = 2;

            QL_REQUIRE(fxMin_*fxMax_ < 0.0,  
                       "root not bracketed: f[" +
                       DoubleFormatter::toString(xMin_) + "," +
                       DoubleFormatter::toString(xMax_) + "] -> [" +
                       DoubleFormatter::toExponential(fxMin_) + "," +
                       DoubleFormatter::toExponential(fxMax_) + "]");

            QL_REQUIRE(guess > xMin_, 
                       "Solver1D: guess (" +
                       DoubleFormatter::toString(guess) + ") < xMin_ (" +
                       DoubleFormatter::toString(xMin_) + ")");
            QL_REQUIRE(guess < xMax_, 
                       "Solver1D: guess (" +
                       DoubleFormatter::toString(guess) + ") > xMax_ (" +
                       DoubleFormatter::toString(xMax_) + ")");

            root_ = guess;

            return impl().solveImpl(f, QL_MAX(QL_FABS(accuracy), QL_EPSILON));
        }

        /*! This method sets the maximum number of function
            evaluations for the bracketing routine. An error is thrown
            if a bracket is not found after this number of
            evaluations.
        */
        void setMaxEvaluations(Size evaluations);
        //! sets the lower bound for the function domain
        void setLowerBound(double lowerBound);
        //! sets the upper bound for the function domain
        void setUpperBound(double upperBound);
        //@}
      protected:
        mutable double root_, xMin_, xMax_, fxMin_, fxMax_;
        Size maxEvaluations_;
        mutable Size evaluationNumber_;
        // core of the Barton-Nackman trick
        Impl& impl() { return static_cast<Impl&>(*this); }
        const Impl& impl() const { return static_cast<const Impl&>(*this); }
      private:
        double enforceBounds_(double x) const;
        double lowerBound_, upperBound_;
        bool lowerBoundEnforced_, upperBoundEnforced_;
    };


    // inline definitions

    template <class T>
    inline void Solver1D<T>::setMaxEvaluations(Size evaluations) {
        maxEvaluations_ = evaluations;
    }

    template <class T>
    inline void Solver1D<T>::setLowerBound(double lowerBound) {
        lowerBound_ = lowerBound;
        lowerBoundEnforced_ = true;
    }

    template <class T>
    inline void Solver1D<T>::setUpperBound(double upperBound) {
        upperBound_ = upperBound;
        upperBoundEnforced_ = true;
    }

    template <class T>
    inline double Solver1D<T>::enforceBounds_(double x) const {
        if (lowerBoundEnforced_ && x < lowerBound_)
            return lowerBound_;
        if (upperBoundEnforced_ && x > upperBound_)
            return upperBound_;
        return x;
    }

}


#endif
