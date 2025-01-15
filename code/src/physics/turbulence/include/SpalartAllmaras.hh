/*------------------------------------------------------------------------*\
**  
**  @file:      SpalartAllmaras.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Header file for Spalart-Allmaras RANS turbulence model
**
\*------------------------------------------------------------------------*/

#ifndef _SPALARTALLMARAS_HH_
#define _SPALARTALLMARAS_HH_

#include "RANS.hh"

namespace PHYSICS
{

/*------------------------------------------------------------------------*\
**  Class SpalartAllmaras Declaration
\*------------------------------------------------------------------------*/

class SpalartAllmaras : public RANS
{
public:
    // Constructors
        SpalartAllmaras(std::weak_ptr<SOLVER::Solver>);
    
    // Virtual Destructor
        virtual ~SpalartAllmaras() {}
    
protected:

    // Member data

};

} // namespace PHYSICS

#endif // _SPALARTALLMARAS_HH_
