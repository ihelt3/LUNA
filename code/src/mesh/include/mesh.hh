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

namespace MESH {

/*------------------------------------------------------------------------*\
**  Class mesh Declaration
\*------------------------------------------------------------------------*/

class mesh
{
public:
    // Constructor
        // Construct an empty Mesh
        mesh() {};
        // Construct from dimension, elements, nodes, BCs, elementConnectivity, BCConnectivity
        mesh(int, std::vector<std::shared_ptr<element>>, std::vector<std::shared_ptr<face>>, std::vector<std::shared_ptr<node>>, std::vector<std::shared_ptr<Boundary>>);

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
        const std::vector<std::shared_ptr<element>>& get_elements() const { return _elements; };
        // return nodes vector
        const std::vector<std::shared_ptr<node>>& get_nodes() const { return _nodes; };
        // return BCs vector
        const std::vector<std::shared_ptr<Boundary>>& get_boundaries() const { return _boundaries; };
        // return faces vector
        const std::vector<std::shared_ptr<face>>& get_faces() const { return _faces; };

    // Operator Overloading
        // Overloaded << operator
        friend std::ostream& operator<<(std::ostream&, const mesh&);

    // friend classes
        // Reading classes needs to access private members
        friend class read_base;
        friend class read_su2;
        // mesh adaption classes needs to access and modify protected members
        friend class meshAdaption;
        

protected:
    // Private Member Data
        // dimension of mesh
        int _dimension; 
        // Mesh data
        int _nElements, _nNodes, _nBCs;
        // Elements vector
        std::vector<std::shared_ptr<element>> _elements;
        // Faces vector
        std::vector<std::shared_ptr<face>> _faces;
        // Nodes Vector
        std::vector<std::shared_ptr<node>> _nodes;
        // Boundaries Vector
        std::vector<std::shared_ptr<Boundary>> _boundaries;  


};

}

#endif // _MESH_HH_