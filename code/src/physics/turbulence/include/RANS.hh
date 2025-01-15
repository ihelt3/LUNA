/*------------------------------------------------------------------------*\
**  
**  @file:      RANS.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header file for Reynolds Averaged Navier Stokes turbulence
**              model base class
**
\*------------------------------------------------------------------------*/

#include <memory>

#include "Solver.hh"

#ifndef _RANS_HH_
#define _RANS_HH_

namespace PHYSICS {

/*------------------------------------------------------------------------*\
**  Class RANS Declaration
\*------------------------------------------------------------------------*/

// Template class for flow solvers
class RANS
{
public:
    // Constructors
        // Construct with solver
        RANS(std::weak_ptr<SOLVER::Solver>);

    // Virtual Destructor
    virtual ~RANS() {}

    // Virtual Methods


    // Common get methods
        // Get Eddy Viscosity
        std::vector<double> get_eddyViscosity() const { return _eddyViscosity; }
        double get_eddyViscosity(int idx) const { return _eddyViscosity[idx]; }


protected:

    // Member data
        // pointer to solver object
        std::weak_ptr<SOLVER::Solver> _solver;
        // Eddy Viscosity
        std::vector<double> _eddyViscosity;

};

}

#endif // _RANS_HH_