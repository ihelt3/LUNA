/*------------------------------------------------------------------------*\
**  
**  @file:      mesh.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for mesh class
**
\*------------------------------------------------------------------------*/

#include "MeshEntities.hh"
#include "mesh.hh"

/*------------------------------------------------------------------------*\
**  Class mesh Implementation
\*------------------------------------------------------------------------*/


// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //

// Construct from dimension, elements, faces, nodes and BCs
MESH::mesh::mesh(int dimension, std::vector<std::shared_ptr<element>> elements, 
                                std::vector<std::shared_ptr<face>> faces, 
                                std::vector<std::shared_ptr<node>> nodes, 
                                std::vector<std::shared_ptr<Boundary>> BCs)
:
    _dimension(dimension),
    _elements(elements),
    _faces(faces),
    _nodes(nodes),
    _boundaries(BCs)
{}


// * * * * * * * * * * * * * *  Get boundary id from name * * * * * * * * * * * * * * * //
int MESH::mesh::get_boundaryID(std::string name) const {
    for (int i = 0; i < _boundaries.size(); i++) {
        if (_boundaries[i]->get_name() == name) {
            return _boundaries[i]->get_id();
        }
    }
    std::cerr << "Boundary " << name << " not found" << std::endl;
    return 0; // Note: Boundaries always have negative IDs
}

// * * * * * * * * * * * * * *  Get boundary index from name * * * * * * * * * * * * * * * //
int MESH::mesh::get_boundaryIdx(std::string name) const {
    for (int i = 0; i < _boundaries.size(); i++) {
        if (_boundaries[i]->get_name() == name) {
            return i;
        }
    }
    std::cerr << "Boundary " << name << " not found" << std::endl;
    return -1; // Note: boundary indexes are always natural numbers
}

// * * * * * * * * * * * * * *  Get boundary index from ID * * * * * * * * * * * * * * * //
int MESH::mesh::get_boundaryIdx(int id) const {
    for (int i = 0; i < _boundaries.size(); i++) {
        if (_boundaries[i]->get_id() == id) {
            return i;
        }
    }
    std::cerr << "Boundary ID " << id << " not found" << std::endl;
    return -1; // Note: Boundaries indexes are always natural numbers
}

// * * * * * * * * * * * * * *  Overloaded << operator * * * * * * * * * * * * * * * //
std::ostream& MESH::operator<<(std::ostream& os, const MESH::mesh& mesh) {
    os << "Dimension: " << mesh.get_dimension() << std::endl;
    os << "Elements: " << mesh.get_elements().size() << std::endl;
    os << "Faces: " << mesh.get_faces().size() << std::endl;
    os << "Nodes: " << mesh.get_nodes().size() << std::endl;
    os << "Boundaries: " << mesh.get_boundaries().size() << std::endl;
    return os;
}