
/*
 Copyright (C) 2004 Ferdinando Ametrano

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

/*! \file jumpdiffusionengine.hpp
    \brief Jump diffusion (Merton 1976) engine
*/

#ifndef quantlib_jumpdiffusionengine_h
#define quantlib_jumpdiffusionengine_h

#include <ql/PricingEngines/Vanilla/vanillaengines.hpp>

namespace QuantLib {

    class JumpDiffusionEngine : public VanillaEngine {
      public:
        JumpDiffusionEngine(const Handle<VanillaEngine>&);
        void calculate() const;
      private:
        Handle<VanillaEngine> baseEngine_;
    };




}





#endif
