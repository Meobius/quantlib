
/*
 Copyright (C) 2003 Ferdinando Ametrano

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

/*! \file localvolsurface.hpp
    \brief Local volatility surface derived from a Black vol surface
*/

#ifndef quantlib_localvolsurface_hpp
#define quantlib_localvolsurface_hpp

#include <ql/voltermstructure.hpp>
#include <ql/termstructure.hpp>

namespace QuantLib {

    namespace VolTermStructures {

        //! Local volatility surface derived from a Black vol surface
        /*! For details about this implementation refers to
            "Stochastic Volatility and Local Volatility", by Jim Gatheral
            www.math.nyu.edu/fellows_fin_math/gatheral/Lecture1_Fall02.pdf
        */
        class LocalVolSurface : public LocalVolTermStructure,
                                public Patterns::Observer {
          public:
            LocalVolSurface(
                const RelinkableHandle<BlackVolTermStructure>& blackTS,
                const RelinkableHandle<TermStructure>& riskFreeTS,
                const RelinkableHandle<TermStructure>& dividendTS,
                const RelinkableHandle<MarketElement>& underlying);
            LocalVolSurface(
                const RelinkableHandle<BlackVolTermStructure>& blackTS,
                const RelinkableHandle<TermStructure>& riskFreeTS,
                const RelinkableHandle<TermStructure>& dividendTS,
                double underlying);
            //! \name LocalVolTermStructure interface
            //@{
            Date referenceDate() const {
                return blackTS_->referenceDate();
            }
            DayCounter dayCounter() const {
                return blackTS_->dayCounter();
            }
            Date maxDate() const { return blackTS_->maxDate(); }
            //@}
            //! \name Observer interface
            //@{
            void update() { notifyObservers(); }
            //@}
            //! \name Visitability
            //@{
            virtual void accept(Patterns::AcyclicVisitor&);
            //@}
          protected:
            double localVolImpl(Time, double, bool extrapolate) const;
          private:
            RelinkableHandle<BlackVolTermStructure> blackTS_;
            RelinkableHandle<TermStructure> riskFreeTS_, dividendTS_;
            RelinkableHandle<MarketElement> underlying_;
        };

    }

}


#endif
