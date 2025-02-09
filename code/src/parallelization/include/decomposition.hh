/*------------------------------------------------------------------------*\
**  
**  @file:      decomposition.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header file for decomposition class
**
\*------------------------------------------------------------------------*/

#ifndef _DECOMPOSITION_HH_
#define _DECOMPOSITION_HH_

#include <vector>

#include "mesh.hh"

namespace PARALLEL
{


/*------------------------------------------------------------------------*\
**  Class decomposition Declaration
\*------------------------------------------------------------------------*/
class decomposition
{
public:
    decomposition(MESH::mesh &mesh);
    ~decomposition();

protected:
    // Member Data
        // Original mesh
        MESH::mesh &_mesh;
        // Decomposed meshes
        std::vector<MESH::mesh> _meshes;
};

}



#endif // _DECOMPOSITION_HH_