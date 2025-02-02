/*------------------------------------------------------------------------*\
**  
**  @file:      outlet.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for velocity outlet boundary condition
**
\*------------------------------------------------------------------------*/

#include "BoundaryConditions.hh"
#include "outlet.hh"


/*------------------------------------------------------------------------*\
**  Class inlet Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //
// Construct with BC copy constructor
BOUNDARIES::outlet::outlet(std::weak_ptr<SOLVER::Solver> solver, std::string BCname, double pressure) 
: 
    BOUNDARIES::BoundaryCondition(solver, BCname)
{
    _pressure = pressure;
}

// Construct with name, faces vector and ID
BOUNDARIES::outlet::outlet(std::weak_ptr<SOLVER::Solver> solver , int BCID, double pressure)
: 
    BOUNDARIES::BoundaryCondition(solver, BCID) 
{
    _pressure = pressure; 
};


// * * * * * * * * * * * * * *  Check if BC is complete * * * * * * * * * * * * * * * //
bool BOUNDARIES::outlet::isComplete()
{
    return true;
}

// * * * * * * * * * * * * * *  Get Pressure for specific face * * * * * * * * * * * * * * * //
double BOUNDARIES::outlet::get_pressure(int globalFaceIdx)
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx]->onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    // return pressure
    return _pressure;
}

// * * * * * * * * * * * * * *  Get Velocity for specific face * * * * * * * * * * * * * * * //
MATH::Vector BOUNDARIES::outlet::get_velocity(int globalFaceIdx)
{
    auto solverPtr = get_solver();

    // Make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx]->onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    // Get velocity from nearest internal cell
    int internalID = solverPtr->get_mesh()->get_faces()[globalFaceIdx]->get_elements()[0]->get_id();
    MATH::Vector v = solverPtr->get_cellVelocityField().get_internal()[internalID];

    return v;
}

// * * * * * * * * * * * * * *  Get Mass Flux * * * * * * * * * * * * * * * //
double BOUNDARIES::outlet::get_massFlux(int globalFaceIdx)
{
    // Lock weak pointer to create shared pointer
    auto solverPtr = get_solver();

    // Make sure face is on the boundary
    assert( solverPtr->get_mesh()->get_boundaries()[_bcIdx]->onBoundary(globalFaceIdx) && "Face is not on boundary!" );

    // Get mesh elements
    std::shared_ptr<MESH::face> f = solverPtr->get_mesh()->get_faces()[globalFaceIdx];
    std::shared_ptr<MESH::element> cell = f->get_elements()[0];

    // Get the velocity from the nearest internal element
    MATH::Vector v = get_velocity(globalFaceIdx);

    // Mass flux going INTO the cell
    return - ( solverPtr->get_density() * v * cell->get_normals()[*cell == *f] * f->get_volume() );
}