
/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2003 RiskMap srl

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

#include "covariance.hpp"
#include <ql/MonteCarlo/getcovariance.hpp>
#include <ql/Math/symmetricschurdecomposition.hpp>
#include <ql/Math/sequencestatistics.hpp>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

using namespace QuantLib;

#define LENGTH(a) (sizeof(a)/sizeof(a[0]))

CppUnit::Test* CovarianceTest::suite() {
    CppUnit::TestSuite* tests =
        new CppUnit::TestSuite("Covariance/correlation tests");
    tests->addTest(new CppUnit::TestCaller<CovarianceTest>
                   ("Testing correlation-salvaging algorithms",
                    &CovarianceTest::testSalvagingCorrelation));
    tests->addTest(new CppUnit::TestCaller<CovarianceTest>
                   ("Testing covariance calculation",
                    &CovarianceTest::testCovariance));
    return tests;
}

void CovarianceTest::testSalvagingCorrelation() {

    double expected, calculated;

    Size i, j, n = 3;

    Matrix badCorr(n, n);
    badCorr[0][0] = 1.0; badCorr[0][1] = 0.9; badCorr[0][2] = 0.7;
    badCorr[1][0] = 0.9; badCorr[1][1] = 1.0; badCorr[1][2] = 0.3;
    badCorr[2][0] = 0.7; badCorr[2][1] = 0.3; badCorr[2][2] = 1.0;

    Array expEigenValues(n), calcEigenValues(n);
    expEigenValues[0]= 2.29672779250086000;
    expEigenValues[1]= 0.71062464690503600;
    expEigenValues[2]=-0.00735243940589643;

    Matrix expEigenVectors(n, n), calcEigenVectors(n, n);
    expEigenVectors[0][0] = 0.659916844979967;
    expEigenVectors[0][1] =-0.074153598758056;
    expEigenVectors[0][2] = 0.747670382924798;
    expEigenVectors[1][0] = 0.570995981829225;
    expEigenVectors[1][1] =-0.597285600732372;
    expEigenVectors[1][2] =-0.563217098366739;
    expEigenVectors[2][0] = 0.488337328541005;
    expEigenVectors[2][1] = 0.798593234975728;
    expEigenVectors[2][2] =-0.351817422258245;

    SymmetricSchurDecomposition schur(badCorr);

    calcEigenValues = schur.eigenvalues();
    calcEigenVectors = schur.eigenvectors();

    Matrix nullMatrix = calcEigenVectors * transpose(calcEigenVectors);
    nullMatrix[0][0] -= 1.0;
    nullMatrix[1][1] -= 1.0;
    nullMatrix[2][2] -= 1.0;

    Matrix nullMatrix2(n, n, 0.0);
    nullMatrix2[0][0] = calcEigenValues[0];
    nullMatrix2[1][1] = calcEigenValues[1];
    nullMatrix2[2][2] = calcEigenValues[2];
    nullMatrix2 = calcEigenVectors * nullMatrix2;
    nullMatrix2 -= (badCorr * calcEigenVectors);

    for (i=0; i<n; i++) {
        calculated = calcEigenValues[i];
        expected   =  expEigenValues[i];
        if (QL_FABS(calculated-expected) > 1.0e-10)
            CPPUNIT_FAIL("SymmetricSchurDecomposition "
                         "eigenvalues[" + IntegerFormatter::toString(i) + "]"
                         "    calculated: "
                         + DoubleFormatter::toString(calculated,16) + "\n"
                         "    expected:   "
                         + DoubleFormatter::toString(expected,16));
        for (j=0; j<n; j++) {
            calculated = calcEigenVectors[i][j];
            expected   =  expEigenVectors[i][j];
            if (QL_FABS(calculated-expected) > 1.0e-10)
                CPPUNIT_FAIL("SymmetricSchurDecomposition "
                             "eigenvector[" + 
                             IntegerFormatter::toString(i) + "]"
                             "[" + IntegerFormatter::toString(j) + "]:\n"
                             "    calculated: "
                             + DoubleFormatter::toString(calculated,16) + "\n"
                             "    expected:   "
                             + DoubleFormatter::toString(expected,16));
            if (QL_FABS(nullMatrix[i][j]) > 1.0e-10)
                CPPUNIT_FAIL("SymmetricSchurDecomposition "
                             "nullMatrix[" + 
                             IntegerFormatter::toString(i) + "]"
                             "[" + IntegerFormatter::toString(j) + "]:\n"
                             "    calculated: "
                             + DoubleFormatter::toString(nullMatrix[i][j],16) 
                             + "\n"
                             "    expected:   "
                             + DoubleFormatter::toString(0.0,16));
            if (QL_FABS(nullMatrix2[i][j]) > 1.0e-10)
                CPPUNIT_FAIL("SymmetricSchurDecomposition "
                             "nullMatrix2[" + 
                             IntegerFormatter::toString(i) + "]"
                             "[" + IntegerFormatter::toString(j) + "]:\n"
                             "    calculated: "
                             + DoubleFormatter::toString(nullMatrix2[i][j],16) 
                             + "\n"
                             "    expected:   "
                             + DoubleFormatter::toString(0.0,16));
        }
    }

    Matrix goodCorr(n, n);
    goodCorr[0][0] = goodCorr[1][1] = goodCorr[2][2] = 1.00000000000;
    goodCorr[0][1] = goodCorr[1][0] = 0.894024408508599;
    goodCorr[0][2] = goodCorr[2][0] = 0.696319066114392;
    goodCorr[1][2] = goodCorr[2][1] = 0.300969036104592;

    Matrix b = pseudoSqrt(badCorr, Spectral);
//    Matrix b = pseudoSqrt(badCorr, Hypersphere);
    Matrix calcCorr = b * transpose(b);

    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            expected   = goodCorr[i][j];
            calculated = calcCorr[i][j];
            if (QL_FABS(calculated-expected) > 1.0e-10)
                CPPUNIT_FAIL("SalvagingCorrelation with spectral alg"
                             "cor[" + IntegerFormatter::toString(i) + "]"
                             "[" + IntegerFormatter::toString(j) + "]:\n"
                             "    calculated: "
                             + DoubleFormatter::toString(calculated,16) + "\n"
                             "    expected:   "
                             + DoubleFormatter::toString(expected,16));
        }
    }

}

void CovarianceTest::testCovariance() {

    double data00[] = { 3.0,  9.0 };
    double data01[] = { 2.0,  7.0 };
    double data02[] = { 4.0, 12.0 };
    double data03[] = { 5.0, 15.0 };
    double data04[] = { 6.0, 17.0 };
    double* data[5] = { data00, data01, data02, data03, data04 };
    std::vector<double> weights(LENGTH(data), 1.0);
        
    Size i, j, n = LENGTH(data00);

    Matrix expCor(n, n);
    expCor[0][0] = 1.0000000000000000; expCor[0][1] = 0.9970544855015813;
    expCor[1][0] = 0.9970544855015813; expCor[1][1] = 1.0000000000000000;

    SequenceStatistics<> s(n);
    std::vector<double> temp(n);

    for (i = 0; i<LENGTH(data); i++) {
        for (j=0; j<n; j++) {
            temp[j]= data[i][j];
        }
        s.add(temp, weights[i]);
    }


    std::vector<double> m = s.mean();
    std::vector<double> std = s.standardDeviation();
    Matrix calcCov  =  s.covariance();
    Matrix calcCor  =  s.correlation();


    Matrix expCov(n, n);
    for (i=0; i<n; i++) {
        expCov[i][i] = std[i]*std[i];
        for (j=0; j<i; j++) {
            expCov[i][j] = expCov[j][i] = expCor[i][j]*std[i]*std[j];
        }
    }

    double expected, calculated;
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            expected   =  expCor[i][j];
            calculated = calcCor[i][j];
            if (QL_FABS(calculated-expected) > 1.0e-10)
                CPPUNIT_FAIL("SequenceStatistics "
                             "cor[" + IntegerFormatter::toString(i) + "]"
                             "[" + IntegerFormatter::toString(j) + "]:\n"
                             "    calculated: "
                             + DoubleFormatter::toString(calculated,16) + "\n"
                             "    expected:   "
                             + DoubleFormatter::toString(expected,16));

            expected   =  expCov[i][j];
            calculated = calcCov[i][j];
            if (QL_FABS(calculated-expected) > 1.0e-10)
                CPPUNIT_FAIL("SequenceStatistics "
                             "cov[" + IntegerFormatter::toString(i) + "]"
                             "[" + IntegerFormatter::toString(j) + "]:\n"
                             "    calculated: "
                             + DoubleFormatter::toString(calculated,16) + "\n"
                             "    expected:   "
                             + DoubleFormatter::toString(expected,16));
        
        }
    }



    calcCov = getCovariance(std.begin(), std.end(), expCor);

    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            double calculated = calcCov[i][j],
                   expected   = expCov[i][j];
            if (QL_FABS(calculated-expected) > 1.0e-10) {
                CPPUNIT_FAIL("getCovariance "
                             "cov[" + IntegerFormatter::toString(i) + "]"
                             "[" + IntegerFormatter::toString(j) + "]:\n"
                             "    calculated: "
                             + DoubleFormatter::toString(calculated,11) + "\n"
                             "    expected:   "
                             + DoubleFormatter::toString(expected,11));
            }
        }
    }
}

