
/*
 Copyright (C) 2000, 2001, 2002, 2003 Sadruddin Rejeb

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

/*! \file gammadistribution.cpp
    \brief Gamma function
*/

#include <ql/Math/gammadistribution.hpp>

namespace QuantLib {

    double GammaDistribution::operator()(double x) const {
        if (x <= 0.0) return 0.0;

        double gln = GammaFunction().logValue(a_);

        if (x<(a_+1.0)) {
            double ap = a_;
            double del = 1.0/a_;
            double sum = del;
            for (Size n=1; n<=100; n++) {
                ap += 1.0;
                del *= x/ap;
                sum += del;
                if (QL_FABS(del) < QL_FABS(sum)*3.0e-7)
                    return sum*QL_EXP(-x + a_*QL_LOG(x) - gln);
            }
        } else {
            double b = x + 1.0 - a_;
            double c = QL_MAX_DOUBLE;
            double d = 1.0/b;
            double h = d;
            for (Size n=1; n<=100; n++) {
                double an = -1.0*n*(n-a_);
                b += 2.0;
                d = an*d + b;
                if (QL_FABS(d) < QL_EPSILON) d = QL_EPSILON;
                c = b + an/c;
                if (QL_FABS(c) < QL_EPSILON) c = QL_EPSILON;
                d = 1.0/d;
                double del = d*c;
                h *= del;
                if (QL_FABS(del - 1.0)<QL_EPSILON) 
                    return h*QL_EXP(-x + a_*QL_LOG(x) - gln);
            }
        }
        throw Error("Too few iterations in GammaDistribution");
    }

    const double GammaFunction::c1_ = 76.18009172947146;
    const double GammaFunction::c2_ = -86.50532032941677;
    const double GammaFunction::c3_ = 24.01409824083091;
    const double GammaFunction::c4_ = -1.231739572450155;
    const double GammaFunction::c5_ = 0.1208650973866179e-2;
    const double GammaFunction::c6_ = -0.5395239384953e-5;

    double GammaFunction::logValue(double x) const {
        double temp = x + 5.5;
        temp -= (x + 0.5)*QL_LOG(temp);
        double ser=1.000000000190015;
        ser += c1_/(x + 1.0);
        ser += c2_/(x + 2.0);
        ser += c3_/(x + 3.0);
        ser += c4_/(x + 4.0);
        ser += c5_/(x + 5.0);
        ser += c6_/(x + 6.0);

        return -temp+QL_LOG(2.5066282746310005*ser/x);
    }

}
