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
UTILITIES::BoundaryCondition::BoundaryCondition(std::weak_ptr<SOLVER::Solver> solver , std::string BCname)
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
UTILITIES::BoundaryCondition::BoundaryCondition(std::weak_ptr<SOLVER::Solver> solver , int BCID)
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

std::shared_ptr<SOLVER::Solver> UTILITIES::BoundaryCondition::get_solver()
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = _solver.lock();

    if (!solverPtr) {
        std::cerr << "Error: Solver pointer expired" << std::endl;
        throw std::runtime_error("Solver pointer expired");
    }

    return solverPtr;
}


/*------------------------------------------------------------------------*\
**  Class viscousWallBC Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //
// Construct with BC copy constructor
UTILITIES::viscousWallBC::viscousWallBC(std::weak_ptr<SOLVER::Solver> solver , std::string BCname)
: 
    UTILITIES::BoundaryCondition(solver, BCname)
{ 
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    _velocity = MATH::Vector(solverPtr->get_mesh()->get_dimension(), 0.0); 
};
// Construct with name, faces vector and ID
UTILITIES::viscousWallBC::viscousWallBC(std::weak_ptr<SOLVER::Solver> solver , int BCID)
: 
    UTILITIES::BoundaryCondition(solver, BCID) 
{ 
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    _velocity = MATH::Vector(solverPtr->get_mesh()->get_dimension(), 0.0); 
};

// * * * * * * * * * * * * * *  Check if BC is complete * * * * * * * * * * * * * * * //
bool UTILITIES::viscousWallBC::isComplete()
{
    return true;
}

// * * * * * * * * * * * * * *  Get Pressure for specific face * * * * * * * * * * * * * * * //
double UTILITIES::viscousWallBC::get_pressure(int globalFaceIdx)
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx].onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    
    // Get pressure from nearest internal cell
    int internalID = solverPtr->get_mesh()->get_faces()[globalFaceIdx].get_elements()[0];
    double p = solverPtr->get_cellPressureField().get_internal()[internalID];
    return p;
}

// * * * * * * * * * * * * * *  Get Velocity for specific face * * * * * * * * * * * * * * * //
MATH::Vector UTILITIES::viscousWallBC::get_velocity(int globalFaceIdx)
{
    auto solverPtr = get_solver();

    // Make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx].onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    return _velocity;
}

// * * * * * * * * * * * * * *  Get Mass Flux * * * * * * * * * * * * * * * //
double UTILITIES::viscousWallBC::get_massFlux(int globalFaceIdx)
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // Make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx].onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    // Get mesh elements
    MESH::face f = solverPtr->get_mesh()->get_faces()[globalFaceIdx];
    MESH::element cell = solverPtr->get_mesh()->get_elements()[f.get_elements()[0]];

    // Mass flux going INTO the cell
    return - ( solverPtr->get_density() * _velocity * cell.get_normals()[cell == f] * f.get_volume() );
}


