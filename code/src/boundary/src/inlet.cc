/*------------------------------------------------------------------------*\
**  
**  @file:      inlet.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for velocity inlet boundary condition
**
\*------------------------------------------------------------------------*/

#include "BoundaryConditions.hh"
#include "inlet.hh"


/*------------------------------------------------------------------------*\
**  Class inlet Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //
// Construct with BC copy constructor
BOUNDARIES::inlet::inlet(std::weak_ptr<SOLVER::Solver> solver , std::string BCname, MATH::Vector velocity)
: 
    BOUNDARIES::BoundaryCondition(solver, BCname)
{ 
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // Check velocity input
    assert( solverPtr->get_mesh()->get_dimension() == velocity.size() && "Inlet velocity must be same dimension as mesh!" );

    _velocity = velocity; 
};

// Construct with name, faces vector and ID
BOUNDARIES::inlet::inlet(std::weak_ptr<SOLVER::Solver> solver , int BCID, MATH::Vector velocity)
: 
    BOUNDARIES::BoundaryCondition(solver, BCID) 
{ 
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // Check velocity input
    assert( solverPtr->get_mesh()->get_dimension() == velocity.size() && "Inlet velocity must be same dimension as mesh!" );

    _velocity = velocity; 
};


// * * * * * * * * * * * * * *  Check if BC is complete * * * * * * * * * * * * * * * //
bool BOUNDARIES::inlet::isComplete()
{
    return true;
}

// * * * * * * * * * * * * * *  Get Pressure for specific face * * * * * * * * * * * * * * * //
double BOUNDARIES::inlet::get_pressure(int globalFaceIdx)
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
MATH::Vector BOUNDARIES::inlet::get_velocity(int globalFaceIdx)
{
    auto solverPtr = get_solver();

    // Make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx]->onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    return _velocity;
}

// * * * * * * * * * * * * * *  Get Mass Flux * * * * * * * * * * * * * * * //
double BOUNDARIES::inlet::get_massFlux(int globalFaceIdx)
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


