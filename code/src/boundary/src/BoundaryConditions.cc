/*------------------------------------------------------------------------*\
**  
**  @file:      BoundaryConditions.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation file of different boundary condition classes
**
\*------------------------------------------------------------------------*/

#include "BoundaryConditions.hh"
#include "Solver.hh"
#include "cassert"

/*------------------------------------------------------------------------*\
**  Class Boundary Condition Implementation
\*------------------------------------------------------------------------*/

// Initialize with solver and name
BOUNDARIES::BoundaryCondition::BoundaryCondition(std::weak_ptr<SOLVER::Solver> solver , std::string BCname)
:
    _solver(solver),
    _name(BCname)
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // Get Boundary Condition ID from name
    _bcID = solverPtr->get_mesh()->get_boundaryID(_name);
    _bcIdx = solverPtr->get_mesh()->get_boundaryIdx(_name);
}

// Initialize with solver and BC ID
BOUNDARIES::BoundaryCondition::BoundaryCondition(std::weak_ptr<SOLVER::Solver> solver , int BCID)
:
    _solver(solver),
    _bcID(BCID)
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // Get Boundary Condition name from ID
    _bcIdx = solverPtr->get_mesh()->get_boundaryIdx(BCID);
    _name = solverPtr->get_mesh()->get_boundaries()[_bcIdx].get_name();
    
}

std::shared_ptr<SOLVER::Solver> BOUNDARIES::BoundaryCondition::get_solver()
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = _solver.lock();

    if (!solverPtr) {
        std::cerr << "Error: Solver pointer expired" << std::endl;
        throw std::runtime_error("Solver pointer expired");
    }

    return solverPtr;
}

