/*------------------------------------------------------------------------*\
**  
**  @file:      RANS.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation file for Reynolds Averaged Navier Stokes (RANS) 
**              turbulence model base class
**
\*------------------------------------------------------------------------*/

#include "RANS.hh"

/*------------------------------------------------------------------------*\
**  Class RANS Implementation
\*------------------------------------------------------------------------*/

// Constructors

// Construct with solver
PHYSICS::RANS::RANS(std::weak_ptr<SOLVER::Solver> solver) : _solver(solver) {}
