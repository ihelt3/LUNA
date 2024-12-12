/*------------------------------------------------------------------------*\
**  
**  @file:      mesh.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header for mesh class
**
\*------------------------------------------------------------------------*/

#ifndef _MESH_HH_
#define _MESH_HH_

#include <vector>

#include "MeshEntities.hh"

namespace MESH
{

/*------------------------------------------------------------------------*\
**  Class read_base Declaration
\*------------------------------------------------------------------------*/

class mesh
{
public:
    // Constructor
        // Construct from dimension, elements, nodes, BCs, elementConnectivity, BCConnectivity
        mesh(int, std::vector<element>, std::vector<face>, std::vector<node>, std::vector<Boundary>);

    // Member Data
        // dimension of mesh
        int _dimension; 
        // Mesh data
        int _nElements, _nNodes, _nBCs;
        // elements and faces vector
        std::vector<element> _elements;
        std::vector<face> _faces;

        // Nodes Vector
        std::vector<node> _nodes;
        // Boundary Conditions Vector
        std::vector<Boundary> _boundaries;  
        // element Connectivity tensor


    // Member Functions
        // Get boundary ID from name
        int get_boundaryID(std::string) const;
        // Get Boundary index from name
        int get_boundaryIdx(std::string) const;
        // Get Boundary index from ID
        int get_boundaryIdx(int) const;

    // get methods
        const int& get_dimension() const { return _dimension; };
        // return elements vector
        const std::vector<element>& get_elements() const { return _elements; };
        // return nodes vector
        const std::vector<node>& get_nodes() const { return _nodes; };
        // return BCs vector
        const std::vector<Boundary>& get_boundaries() const { return _boundaries; };
        // return faces vector
        const std::vector<face>& get_faces() const { return _faces; };

    // set methods


};

}

#endif // _MESH_HH_