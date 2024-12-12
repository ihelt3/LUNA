/*------------------------------------------------------------------------*\
**  
**  @file:      mesh.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for mesh class
**
\*------------------------------------------------------------------------*/

#include "mesh.hh"

/*------------------------------------------------------------------------*\
**  Class mesh Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
MESH::mesh::mesh(int dimension, std::vector<element> elements, std::vector<face> _faces, std::vector<node> nodes, std::vector<Boundary> BCs)
:
    _dimension(dimension),
    _elements(elements),
    _faces(_faces),
    _nodes(nodes),
    _boundaries(BCs)
{
    // Print mesh statistics
    std::cout << "Elements: " << _elements.size() << std::endl;
    std::cout << "Faces: " << _faces.size() << std::endl;
    std::cout << "Nodes: " << _nodes.size() << std::endl;
};


// * * * * * * * * * * * * * *  Get boundary id from name * * * * * * * * * * * * * * * //
int MESH::mesh::get_boundaryID(std::string name) const {
    for (int i = 0; i < _boundaries.size(); i++) {
        if (_boundaries[i].get_name() == name) {
            return _boundaries[i].get_id();
        }
    }
    std::cerr << "Boundary " << name << " not found" << std::endl;
    return 0; // Note: Boundaries always have negative IDs
}

// * * * * * * * * * * * * * *  Get boundary index from name * * * * * * * * * * * * * * * //
int MESH::mesh::get_boundaryIdx(std::string name) const {
    for (int i = 0; i < _boundaries.size(); i++) {
        if (_boundaries[i].get_name() == name) {
            return i;
        }
    }
    std::cerr << "Boundary " << name << " not found" << std::endl;
    return -1; // Note: boundary indexes are always natural numbers
}

// * * * * * * * * * * * * * *  Get boundary index from ID * * * * * * * * * * * * * * * //
int MESH::mesh::get_boundaryIdx(int id) const {
    for (int i = 0; i < _boundaries.size(); i++) {
        if (_boundaries[i].get_id() == id) {
            return i;
        }
    }
    std::cerr << "Boundary ID " << id << " not found" << std::endl;
    return -1; // Note: Boundaries indexes are always natural numbers
}