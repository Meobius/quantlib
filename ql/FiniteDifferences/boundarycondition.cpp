
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

/*! \file boundarycondition.cpp
    \brief boundary conditions for differential operators
*/

#include <ql/FiniteDifferences/boundarycondition.hpp>

namespace QuantLib {

    namespace FiniteDifferences {

        // Neumann conditions
        
        NeumannBC::NeumannBC(double value, NeumannBC::Side side)
        : value_(value), side_(side) {}

        void NeumannBC::applyBeforeApplying(TridiagonalOperator& L) const {
            switch (side_) {
              case Lower:
                L.setFirstRow(-1.0,1.0);
                break;
              case Upper:
                L.setLastRow(-1.0,1.0);
                break;
              default:
                throw Error("Unknown side for Neumann boundary condition");
            }
        }

        void NeumannBC::applyAfterApplying(Array& u) const {
            switch (side_) {
              case Lower:
                u[0] = u[1] - value_;
                break;
              case Upper:
                u[u.size()-1] = u[u.size()-2] + value_;
                break;
              default:
                throw Error("Unknown side for Neumann boundary condition");
            }
        }
        
        void NeumannBC::applyBeforeSolving(TridiagonalOperator& L,
                                           Array& rhs) const {
            switch (side_) {
              case Lower:
                L.setFirstRow(-1.0,1.0);
                rhs[0] = value_;
                break;
              case Upper:
                L.setLastRow(-1.0,1.0);
                rhs[rhs.size()-1] = value_;
                break;
              default:
                throw Error("Unknown side for Neumann boundary condition");
            }
        }

        void NeumannBC::applyAfterSolving(Array&) const {}


        // Dirichlet conditions

        DirichletBC::DirichletBC(double value, DirichletBC::Side side)
        : value_(value), side_(side) {}
        
        void DirichletBC::applyBeforeApplying(TridiagonalOperator& L) const {
            switch (side_) {
              case Lower:
                L.setFirstRow(1.0,0.0);
                break;
              case Upper:
                L.setLastRow(0.0,1.0);
                break;
              default:
                throw Error("Unknown side for Neumann boundary condition");
            }
        }

        void DirichletBC::applyAfterApplying(Array& u) const {
            switch (side_) {
              case Lower:
                u[0] = value_;
                break;
              case Upper:
                u[u.size()-1] = value_;
                break;
              default:
                throw Error("Unknown side for Neumann boundary condition");
            }
        }
        
        void DirichletBC::applyBeforeSolving(TridiagonalOperator& L,
                                             Array& rhs) const {
            switch (side_) {
              case Lower:
                L.setFirstRow(1.0,0.0);
                rhs[0] = value_;
                break;
              case Upper:
                L.setLastRow(0.0,1.0);
                rhs[rhs.size()-1] = value_;
                break;
              default:
                throw Error("Unknown side for Neumann boundary condition");
            }
        }

        void DirichletBC::applyAfterSolving(Array&) const {}

    }

}
