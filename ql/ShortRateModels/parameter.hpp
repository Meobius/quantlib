
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

/*! \file parameter.hpp
    \brief Model parameter classes
*/

#ifndef quantlib_interest_rate_modelling_parameter_h
#define quantlib_interest_rate_modelling_parameter_h

#include <ql/qldefines.hpp>
#include <ql/termstructure.hpp>
#include <ql/Optimization/constraint.hpp>

namespace QuantLib {

    namespace ShortRateModels {

        //! Base class for model parameter implementation
        class ParameterImpl {
          public:
            virtual ~ParameterImpl() {}
            virtual double value(const Array& params, Time t) const = 0;
        };

        //! Base class for model arguments
        class Parameter : public Patterns::Bridge<Parameter,ParameterImpl> {
          public:
            Parameter()
            : constraint_(Optimization::NoConstraint()) {}
            const Array& params() const { return params_; }
            void setParam(Size i, double x) { params_[i] = x; }
            bool testParams(const Array& params) const {
                return constraint_.test(params);
            }
            Size size() const { return params_.size(); }
            double operator()(Time t) const {
                return impl_->value(params_, t);
            }
            const Handle<ParameterImpl>& implementation() const {return impl_;}
          protected:
            Parameter(Size size,
                      const Handle<ParameterImpl>& impl,
                      const Optimization::Constraint& constraint)
            : Patterns::Bridge<Parameter,ParameterImpl>(impl),
              params_(size), constraint_(constraint) {}
            Array params_;
            Optimization::Constraint constraint_;
        };

        //! Standard constant parameter \f$ a(t) = a \f$
        class ConstantParameter : public Parameter {
          private:
            class Impl : public Parameter::Impl {
              public:
                double value(const Array& params, Time t) const {
                    return params[0];
                }
            };
          public:
            ConstantParameter(
                const Optimization::Constraint& constraint)
            : Parameter(1,
                        Handle<Parameter::Impl>(new ConstantParameter::Impl),
                        constraint)
            {}

            ConstantParameter(
                double value,
                const Optimization::Constraint& constraint)
            : Parameter(1,
                        Handle<Parameter::Impl>(new ConstantParameter::Impl),
                        constraint) {
                params_[0] = value;
                QL_REQUIRE(testParams(params_),
                           "ConstantParameter: invalid value in constructor");
            }

        };

        //! Parameter which is always zero \f$ a(t) = 0 \f$
        class NullParameter : public Parameter {
          private:
            class Impl : public Parameter::Impl {
              public:
                double value(const Array& params, Time t) const {
                    return 0.0;
                }
            };
          public:
            NullParameter()
            : Parameter(0,
                        Handle<Parameter::Impl>(new NullParameter::Impl),
                        Optimization::NoConstraint())
            {}
        };

        //! Piecewise constant parameter
        /*! \f$ a(t) = a_i if t_{i-1} \geq t < t_i \f$.
            This kind of parameter is usually used to enhance the fitting of a
            model
        */
        class PiecewiseConstantParameter : public Parameter {
          private:
            class Impl : public Parameter::Impl {
              public:
                Impl(const std::vector<Time>& times)
                : times_(times) {}

                double value(const Array& params, Time t) const {
                    Size size = times_.size();
                    for (Size i=0; i<size; i++) {
                        if (t<times_[i])
                            return params[i];
                    }
                    return params[size];
                }
              private:
                std::vector<Time> times_;
            };
          public:
            PiecewiseConstantParameter(const std::vector<Time>& times)
            : Parameter(times.size()+1,
                        Handle<Parameter::Impl>(
                            new PiecewiseConstantParameter::Impl(times)),
                        Optimization::NoConstraint())
            {}
        };

        //! Deterministic time-dependent parameter used for yield-curve fitting
        class TermStructureFittingParameter : public Parameter {
          public:
            class NumericalImpl : public Parameter::Impl {
              public:
                NumericalImpl(
                    const RelinkableHandle<TermStructure>& termStructure)
                : times_(0), values_(0), termStructure_(termStructure) {}

                void set(Time t, double x) {
                    times_.push_back(t);
                    values_.push_back(x);
                }
                void change(double x) {
                    values_.back() = x;
                }
                void reset() {
                    times_.clear();
                    values_.clear();
                }
                double value(const Array& params, Time t) const {
                    std::vector<Time>::const_iterator result =
                        std::find(times_.begin(), times_.end(), t);
                    QL_REQUIRE(result!=times_.end(),
                               "Fitting parameter not set!");
                    return values_[result - times_.begin()];
                }
                const RelinkableHandle<TermStructure>& termStructure() const {
                    return termStructure_;
                }
              private:
                std::vector<Time> times_;
                std::vector<double> values_;
                RelinkableHandle<TermStructure> termStructure_;
            };

            TermStructureFittingParameter(const Handle<Parameter::Impl>& impl)
            : Parameter(0, impl, Optimization::NoConstraint()) {}

            TermStructureFittingParameter(
                const RelinkableHandle<TermStructure>& term)
            : Parameter(0, Handle<Parameter::Impl>(new NumericalImpl(term)),
                        Optimization::NoConstraint())
            {}
        };


    }

}


#endif
