/*------------------------------------------------------------------------*\
**  
**  @file:      wall.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Header for wall boundary condition
**
\*------------------------------------------------------------------------*/

#ifndef _WALL_HH_
#define _WALL_HH_

#include "BoundaryConditions.hh"

namespace BOUNDARIES {

/*------------------------------------------------------------------------*\
**  Class Wall Declaration
\*------------------------------------------------------------------------*/
class viscousWallBC
:
    public BoundaryCondition
{
public:
    // Constructors:
        // Construct with BC copy constructor
        viscousWallBC(std::weak_ptr<SOLVER::Solver> solver , std::string BCname);
        // Construct with name, faces vector and ID
        viscousWallBC(std::weak_ptr<SOLVER::Solver> solver , int BCID);

    // Methods
        // Check if BC is complete
        bool isComplete() override;
        // Set velocity
        void set_velocity(MATH::Vector velocity) override { _velocity = velocity; };

    // Methods to get Boundary condition values
        // Get Pressure From face index and internal pressure field
        double get_pressure(int) override;
        // Get Velocity for specific index
        MATH::Vector get_velocity(int) override;
        // get Mass Flux
        double get_massFlux(int) override;

protected:
    // Member data
        MATH::Vector _velocity; // Assume constant, cartesian aligned velocity for now
        std::vector<double> _pressure; // Pressure is constant at the wall
        boundaryConditionType _bcType = boundaryConditionType::WALL;

};

}

#endif // _WALL_HH_