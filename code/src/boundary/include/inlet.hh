/*------------------------------------------------------------------------*\
**  
**  @file:      inlet.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Header for velocity inlet boundary condition
**
\*------------------------------------------------------------------------*/

#ifndef _INLET_HH_
#define _INLET_HH_

#include "BoundaryConditions.hh"

namespace BOUNDARIES {

/*------------------------------------------------------------------------*\
**  Class Inlet Declaration
\*------------------------------------------------------------------------*/
class inlet
:
    public BoundaryCondition
{
public:
    // Constructor
        // Construct with BC copy constructor
        inlet(std::weak_ptr<SOLVER::Solver> solver, std::string BCname, MATH::Vector);
        // Construct with name, ID
        inlet(std::weak_ptr<SOLVER::Solver> , int, MATH::Vector);

    // Methods
        // Check if BC is complete
        bool isComplete() override;
        
    // Methods to get Boundary condition values
        // Get Pressure From face index and internal pressure field
        double get_pressure(int) override;
        // Get Velocity for specific index
        MATH::Vector get_velocity(int) override;
        // get Mass Flux
        double get_massFlux(int) override;

private:
    // Member data
        // Velocity constant at the inlet
        MATH::Vector _velocity;
        // Pressure varies along inlet
        std::vector<double> _pressure;

};

}

#endif // _INLET_HH_