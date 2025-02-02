/*------------------------------------------------------------------------*\
**  
**  @file:      wall.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation of wall boundary condition
**
\*------------------------------------------------------------------------*/

#include "BoundaryConditions.hh"
#include "wall.hh"


/*------------------------------------------------------------------------*\
**  Class viscousWallBC Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //
// Construct with BC copy constructor
BOUNDARIES::viscousWallBC::viscousWallBC(std::weak_ptr<SOLVER::Solver> solver , std::string BCname)
: 
    BOUNDARIES::BoundaryCondition(solver, BCname)
{ 
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    _velocity = MATH::Vector(solverPtr->get_mesh()->get_dimension(), 0.0); 
};
// Construct with name, faces vector and ID
BOUNDARIES::viscousWallBC::viscousWallBC(std::weak_ptr<SOLVER::Solver> solver , int BCID)
: 
    BOUNDARIES::BoundaryCondition(solver, BCID) 
{ 
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    _velocity = MATH::Vector(solverPtr->get_mesh()->get_dimension(), 0.0); 
};

// * * * * * * * * * * * * * *  Check if BC is complete * * * * * * * * * * * * * * * //
bool BOUNDARIES::viscousWallBC::isComplete()
{
    return true;
}

// * * * * * * * * * * * * * *  Get Pressure for specific face * * * * * * * * * * * * * * * //
double BOUNDARIES::viscousWallBC::get_pressure(int globalFaceIdx)
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx]->onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    
    // Get pressure from nearest internal cell
    int internalID = solverPtr->get_mesh()->get_faces()[globalFaceIdx]->get_elements()[0]->get_id();
    double p = solverPtr->get_cellPressureField().get_internal()[internalID];
    return p;
}

// * * * * * * * * * * * * * *  Get Velocity for specific face * * * * * * * * * * * * * * * //
MATH::Vector BOUNDARIES::viscousWallBC::get_velocity(int globalFaceIdx)
{
    auto solverPtr = get_solver();

    // Make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx]->onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    return _velocity;
}

// * * * * * * * * * * * * * *  Get Mass Flux * * * * * * * * * * * * * * * //
double BOUNDARIES::viscousWallBC::get_massFlux(int globalFaceIdx)
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // Make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx]->onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    // Get mesh elements
    std::shared_ptr<MESH::face> f = solverPtr->get_mesh()->get_faces()[globalFaceIdx];
    std::shared_ptr<MESH::element> cell = f->get_elements()[0];

    // Mass flux going INTO the cell
    return - ( solverPtr->get_density() * _velocity * cell->get_normals()[*cell == *f] * f->get_volume() );
}


