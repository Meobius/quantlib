
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

/*! \file tridiagonaloperator.hpp
    \brief tridiagonal operator
*/

#ifndef quantlib_tridiagonal_operator_h
#define quantlib_tridiagonal_operator_h

#include <ql/array.hpp>
#include <ql/handle.hpp>

namespace QuantLib {

    namespace FiniteDifferences {

        //! Base implementation for tridiagonal operator
        /*! \warning to use real time-dependant algebra, you must overload
            the corresponding operators in the inheriting time-dependent
            class
        */
        class TridiagonalOperator {
            // unary operators
            friend Disposable<TridiagonalOperator> 
            operator+(const TridiagonalOperator&);
            friend Disposable<TridiagonalOperator>
            operator-(const TridiagonalOperator&);
            // binary operators
            friend Disposable<TridiagonalOperator>
            operator+(const TridiagonalOperator&,
                      const TridiagonalOperator&);
            friend Disposable<TridiagonalOperator>
            operator-(const TridiagonalOperator&,
                      const TridiagonalOperator&);
            friend Disposable<TridiagonalOperator>
            operator*(double,
                      const TridiagonalOperator&);
            friend Disposable<TridiagonalOperator>
            operator*(const TridiagonalOperator&,
                      double);
            friend Disposable<TridiagonalOperator>
            operator/(const TridiagonalOperator&,
                      double);
          public:
            typedef Array arrayType;
            // constructors
            TridiagonalOperator(Size size = 0);
            TridiagonalOperator(const Array& low, const Array& mid,
                const Array& high);
            #if defined(QL_PATCH_MICROSOFT)
                /* This copy constructor and assignment operator are here
                   because somehow Visual C++ is not able to generate working
                   ones. They are _not_ to be defined for other compilers
                   which are able to generate correct ones.   */
                TridiagonalOperator(const TridiagonalOperator&);
                TridiagonalOperator& operator=(const TridiagonalOperator&);
            #endif
            TridiagonalOperator(const Disposable<TridiagonalOperator>&);
            TridiagonalOperator& operator=(
                const Disposable<TridiagonalOperator>&);
            //! \name Operator interface
            //@{
            //! apply operator to a given array
            Disposable<Array> applyTo(const Array& v) const;
            //! solve linear system for a given right-hand side
            Disposable<Array> solveFor(const Array& rhs) const;
            //! solve linear system with SOR approach
            Disposable<Array> SOR(const Array& rhs, double tol) const;
            //! identity instance
            static Disposable<TridiagonalOperator> identity(Size size);
            //@}
            //! \name Inspectors
            //@{
            Size size() const;
            bool isTimeDependent();
            const Array& lowerDiagonal() const;
            const Array& diagonal() const;
            const Array& upperDiagonal() const;
            //@}
            //! \name Modifiers
            //@{
            void setFirstRow(double, double);
            void setMidRow(Size, double, double, double);
            void setMidRows(double, double, double);
            void setLastRow(double, double);
            void setTime(Time t);
            //@}
            //! \name Utilities
            //@{
            void swap(TridiagonalOperator&);
            //@}
            //! encapsulation of time-setting logic
            class TimeSetter {
              public:
                virtual ~TimeSetter() {}
                virtual void setTime(Time t,
                                     TridiagonalOperator& L) const = 0;
            };
          protected:
            Array diagonal_, lowerDiagonal_, upperDiagonal_;
            Handle<TimeSetter> timeSetter_;
        };


        // inline definitions

        #if defined(QL_PATCH_MICROSOFT)
            inline TridiagonalOperator::TridiagonalOperator(
                const TridiagonalOperator& L) {
                    lowerDiagonal_ = L.lowerDiagonal_;
                    diagonal_      = L.diagonal_;
                    upperDiagonal_ = L.upperDiagonal_;
                    timeSetter_    = L.timeSetter_;
            }
            inline TridiagonalOperator& TridiagonalOperator::operator=(
                const TridiagonalOperator& L){
                    lowerDiagonal_ = L.lowerDiagonal_;
                    diagonal_      = L.diagonal_;
                    upperDiagonal_ = L.upperDiagonal_;
                    timeSetter_    = L.timeSetter_;
                    return *this;
            }
        #endif

        inline TridiagonalOperator::TridiagonalOperator(
                    const Disposable<TridiagonalOperator>& from) {
            swap(const_cast<Disposable<TridiagonalOperator>&>(from));
        }
        
        inline TridiagonalOperator& TridiagonalOperator::operator=(
                    const Disposable<TridiagonalOperator>& from) {
            swap(const_cast<Disposable<TridiagonalOperator>&>(from));
            return *this;
        }

        inline Size TridiagonalOperator::size() const {
            return diagonal_.size();
        }

        inline bool TridiagonalOperator::isTimeDependent() {
            return !timeSetter_.isNull();
        }

        inline const Array& TridiagonalOperator::lowerDiagonal() const{
            return lowerDiagonal_;
        }

        inline const Array& TridiagonalOperator::diagonal() const{
            return diagonal_;
        }

        inline const Array& TridiagonalOperator::upperDiagonal() const{
            return upperDiagonal_;
        }

        inline void TridiagonalOperator::setFirstRow(double valB,
                                                     double valC) {
            diagonal_[0]      = valB;
            upperDiagonal_[0] = valC;
        }

        inline void TridiagonalOperator::setMidRow(Size i,
                                                   double valA, 
                                                   double valB, 
                                                   double valC) {
            QL_REQUIRE(i>=1 && i<=size()-2,
                "out of range in TridiagonalSystem::setMidRow");
            lowerDiagonal_[i-1] = valA;
            diagonal_[i]        = valB;
            upperDiagonal_[i]   = valC;
        }

        inline void TridiagonalOperator::setMidRows(double valA,
                                                    double valB, 
                                                    double valC) {
            for (Size i=1; i<=size()-2; i++) {
                lowerDiagonal_[i-1] = valA;
                diagonal_[i]        = valB;
                upperDiagonal_[i]   = valC;
            }
        }

        inline void TridiagonalOperator::setLastRow(double valA,
                                                    double valB) {
            lowerDiagonal_[size()-2] = valA;
            diagonal_[size()-1]      = valB;
        }

        inline void TridiagonalOperator::setTime(Time t) {
            if (!timeSetter_.isNull())
                timeSetter_->setTime(t,*this);
        }

        inline void TridiagonalOperator::swap(TridiagonalOperator& from) {
            diagonal_.swap(from.diagonal_);
            lowerDiagonal_.swap(from.lowerDiagonal_);
            upperDiagonal_.swap(from.upperDiagonal_);
            std::swap(timeSetter_,from.timeSetter_);
        }


        // Time constant algebra

        inline Disposable<TridiagonalOperator>
        operator+(const TridiagonalOperator& D) {
            TridiagonalOperator D1 = D;
            return D1;
        }

        inline Disposable<TridiagonalOperator>
        operator-(const TridiagonalOperator& D) {
            Array low = -D.lowerDiagonal_, mid = -D.diagonal_,
                  high = -D.upperDiagonal_;
            TridiagonalOperator result(low,mid,high);
            return result;
        }

        inline Disposable<TridiagonalOperator>
        operator+(const TridiagonalOperator& D1,
                  const TridiagonalOperator& D2) {
            Array low = D1.lowerDiagonal_+D2.lowerDiagonal_,
                  mid = D1.diagonal_+D2.diagonal_,
                  high = D1.upperDiagonal_+D2.upperDiagonal_;
            TridiagonalOperator result(low,mid,high);
            return result;
        }

        inline Disposable<TridiagonalOperator>
        operator-(const TridiagonalOperator& D1,
                  const TridiagonalOperator& D2) {
            Array low = D1.lowerDiagonal_-D2.lowerDiagonal_,
                  mid = D1.diagonal_-D2.diagonal_,
                  high = D1.upperDiagonal_-D2.upperDiagonal_;
            TridiagonalOperator result(low,mid,high);
            return result;
        }

        inline Disposable<TridiagonalOperator> 
        operator*(double a,
                  const TridiagonalOperator& D) {
            Array low = D.lowerDiagonal_*a, mid = D.diagonal_*a,
                  high = D.upperDiagonal_*a;
            TridiagonalOperator result(low,mid,high);
            return result;
        }

        inline Disposable<TridiagonalOperator>
        operator*(const TridiagonalOperator& D,
                  double a) {
            Array low = D.lowerDiagonal_*a, mid = D.diagonal_*a,
                  high = D.upperDiagonal_*a;
            TridiagonalOperator result(low,mid,high);
            return result;
        }

        inline Disposable<TridiagonalOperator>
        operator/(const TridiagonalOperator& D,
                  double a) {
            Array low = D.lowerDiagonal_/a, mid = D.diagonal_/a,
                  high = D.upperDiagonal_/a;
            TridiagonalOperator result(low,mid,high);
            return result;
        }

    }

}


#endif
