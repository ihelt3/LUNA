/*------------------------------------------------------------------------*\
**  
**  @file:      outlet.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Header for outlet boundary condition
**
\*------------------------------------------------------------------------*/

#ifndef _OUTLET_HH_
#define _OUTLET_HH_

#include "BoundaryConditions.hh"

namespace BOUNDARIES {

/*------------------------------------------------------------------------*\
**  Class Inlet Declaration
\*------------------------------------------------------------------------*/
class outlet
:
    public BoundaryCondition
{
public:
    // Constructors
        // Construct with a BC name and pressure
        outlet(std::weak_ptr<SOLVER::Solver> solver, std::string BCname, double pressure);
        // Construct with ID and pressure
        outlet(std::weak_ptr<SOLVER::Solver> solver , int BCID, double pressure);

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
        double _pressure;
};

}

#endif // _OUTLET_HH_