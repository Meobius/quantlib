
/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file grid.cpp
    \brief Grid classes with useful constructors for trees and finite diffs

    \fullpath
    ql/%grid.cpp
*/

// $Id$

#include <ql/grid.hpp>

namespace {

    class CloseEnoughTo : public std::unary_function<double,bool> {
      public:
        CloseEnoughTo(double target, double tolerance = 1.0e-10)
        : target_(target), tolerance_(tolerance) {}
        bool operator()(double x) const {
            return QL_FABS(x-target_) <= tolerance_;
        }
      private:
        double target_, tolerance_;
    };

}

namespace QuantLib {

    Size TimeGrid::findIndex(Time t) const {
        const_iterator result = std::find_if(begin(), end(), 
                                             CloseEnoughTo(t));
        if (result == end()) {
            int i;
            for (i=0; i<int(size()); i++) {
                if ((*this)[i] > t)
                    break;
            }
            if (i == 0) {
                throw Error("Using inadequate TimeGrid: all nodes "
                            "are later than the required time t = "
                            + DoubleFormatter::toString(t,12) +
                            " (earliest node is t1 = "
                            + DoubleFormatter::toString((*this)[0],12) +
                            ")");
            } else if (i == int(size())) {
                throw Error("Using inadequate TimeGrid: all nodes "
                            "are earlier than the required time t = "
                            + DoubleFormatter::toString(t,12) +
                            " (latest node is t1 = "
                            + DoubleFormatter::toString((*this)[size()-1],12) +
                            ")");
            } else {
                throw Error("Using inadequate TimeGrid: the nodes closest "
                            "to the required time t = "
                            + DoubleFormatter::toString(t,12) +
                            " are t1 = " 
                            + DoubleFormatter::toString((*this)[i-1],12) +
                            " and t2 = "
                            + DoubleFormatter::toString((*this)[i],12));
            }
        }
        return result - begin();
    }

}

