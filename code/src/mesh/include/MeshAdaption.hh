/*------------------------------------------------------------------------*\
**  
**  @file:      MeshAdaption.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header for mesh adaption class
**
\*------------------------------------------------------------------------*/

#ifndef _MESHADAPTION_HH_
#define _MESHADAPTION_HH_

#include <memory>

#include "mesh.hh"

namespace MESH
{

/*------------------------------------------------------------------------*\
**  Class meshAdaption Declaration
\*------------------------------------------------------------------------*/

class meshAdaption
{
public:
    // Constructor
        // Construct mesh pointer
        meshAdaption(std::shared_ptr<mesh> meshPtr);

    // Member functions
        // Adapt specific cell
        void refineCell(int cellID);

protected:
    std::shared_ptr<mesh> _meshPtr;

};



}

#endif // _MESHADAPTION_HH_
