/*------------------------------------------------------------------------*\
**  
**  @file:      MeshEntities.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     mesh entities defintions
**
\*------------------------------------------------------------------------*/

#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>
#include <functional>
#include <chrono>

#include "MeshEntities.hh"


/*------------------------------------------------------------------------*\
**  Class mesh_entity Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //

// Construct from node IDs
//    NOTE: This constructor requires the initialize method to be used after instantiation and node vector is added
MESH::mesh_entity::mesh_entity(int id,elementTypeEnum elementType, std::vector<int> nodeIDs, bool subElement)
:
    _id(id),
    _elementType(elementType),
    _nodeIDs(nodeIDs),
    _nodes({}),
    _subElement(subElement),
    _centroid({})
{

}

// Construct from node vector
MESH::mesh_entity::mesh_entity(int id, elementTypeEnum elementType, std::vector<std::weak_ptr<node>> nodes, bool subElement)
:
    _id(id),
    _elementType(elementType),
    _nodes(nodes),
    _subElement(subElement),
    _centroid({})
{
    std::vector<std::shared_ptr<node>> shared_nodes = return_shared(&_nodes);

    // Define node IDs from node vector
    for (int i=0 ; i<_nodes.size() ; i++) {
        _nodeIDs.push_back(shared_nodes[i]->get_id());
    }

    // Run initialize method
    initialize();
}

// construct from face vector
MESH::mesh_entity::mesh_entity(int id, elementTypeEnum elementType, std::vector<std::weak_ptr<face>> faces, bool subElement)
:
    _id(id),
    _elementType(elementType),
    _subElement(subElement),
    _centroid({})
{
    std::vector<std::shared_ptr<face>> shared_faces = return_shared(&faces);

    // Grab nodes from faces
    for (int i=0 ; i<faces.size() ; i++) {
        for (int j=0 ; j<shared_faces[i]->get_nodes().size() ; j++) {
            if (std::find(_nodeIDs.begin(), _nodeIDs.end(), shared_faces[i]->get_nodes()[j]->get_id()) == _nodeIDs.end()) {
                _nodes.push_back(shared_faces[i]->get_nodes()[j]);
                _nodeIDs.push_back(shared_faces[i]->get_nodes()[j]->get_id());
            }
        }
    }

    // Run Initilize vector
    initialize();
}

// * * * * * * * * * * * * * *  initialize * * * * * * * * * * * * * * * //
// Initializer of face class
void MESH::mesh_entity::initialize() {

    // Calculate volume and centroid
    calculateVolume();
    calculateCentroid();
}


// * * * * * * * * * * * * * *  returnQuadDiags * * * * * * * * * * * * * * * //
// return the diaganals of a quadrilateral element
std::vector<std::vector<MESH::node>> MESH::mesh_entity::returnQuadDiags(std::vector<node> nodes)
{
    assert(nodes.size() == 4 && "Expected 4 nodes in vector to get quadrilateral diagonals");
    
    // Put node coordinates in vector
    std::vector<MATH::Vector> coords;
    for (int i=0 ; i<nodes.size() ; i++) {
        coords.push_back(nodes[i].get_coordinates());
    }

    // Get node order using Jarvis March
    std::vector<int> order = MATH::jarvis_march(coords);

    std::ostringstream error_msg;
    error_msg << "Error obtaining convex hull of quadrilateral on element" << _id;
    assert(order.size() == 4 && error_msg.str().c_str());

    std::vector<std::vector<node>> diags = { {nodes[order[0]],nodes[order[2]]} , {nodes[order[1]],nodes[order[3]]} };

    return diags;
}

// * * * * * * * * * * * * * *  triArea * * * * * * * * * * * * * * * //
double MESH::mesh_entity::triArea(std::vector<node> nodes)
{
    double s, a, b, c;
    a = nodes[0]*nodes[1];
    b = nodes[1]*nodes[2];
    c = nodes[2]*nodes[0];
    s = 0.5*(a+b+c);
    // Heron's formula
    return std::sqrt(s*(s-a)*(s-b)*(s-c));
}

// * * * * * * * * * * * * * *  2D Area * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * *  calculateVolume * * * * * * * * * * * * * * * //
void MESH::mesh_entity::calculateVolume() 
{
    std::vector<std::shared_ptr<node>> nodes = return_shared(&_nodes);

    // LINE Volume = Length
    if ( type2dimension[_elementType] == 1 ) {
        // Overloaded * operator on two pointers
        _volume = *nodes[0]* *nodes[1];
    }

    // Generalized approach for 2D elements
    else if ( type2dimension[_elementType] == 2 ) {
        // Use Jarvis March to get convex hull
        std::vector<MATH::Vector> coords;
        for (int i=0 ; i<_nodes.size() ; i++) {
            coords.push_back(nodes[i]->get_coordinates());
        }
        std::vector<int> order = MATH::jarvis_march(coords);

        // Use element order to calculate area
        _volume = 0.0;
        for (int i=0 ; i<order.size()-2 ; i++) {
            _volume += triArea( {*nodes[order[0]],*nodes[order[i+1]],*nodes[order[i+2]]} );
        }

    }

    else if ( type2dimension[_elementType] == 3 ) {
        std::cerr << "3D elements volume calculations not yet supported" << std::endl;
    }

}

// * * * * * * * * * * * * * * Calculate Cell Centroid * * * * * * * * * * * * * * * //
void MESH::mesh_entity::calculateCentroid() 
{
    // Lock nodes
    std::vector<std::shared_ptr<node>> nodes = return_shared(&_nodes);

    MATH::Vector center(nodes[0]->get_coordinates().size(),0.0);
    double c = _nodes.size();
    // Just add all the nodes index wise and divide by number of nodes
    for (int i=0 ; i<_nodes.size() ; i++) {
        center = center + nodes[i]->get_coordinates();
    }
    center = center * (1.0/c);

    _centroid = center;
}


// * * * * * * * * * * * * * * Hash Function * * * * * * * * * * * * * * * //
void MESH::mesh_entity::hash() 
{    
    // Hash the (sorted) node IDs (to ensure order-invariance)
    std::ostringstream oss;

    std::vector<int> sortedNodeIDs = _nodeIDs;
    std::sort(sortedNodeIDs.begin(), sortedNodeIDs.end());
    for (const int& id : sortedNodeIDs) {
        oss << id << "-";
    }

    seed = oss.str();
}

// * * * * * * * * * * * * * * * * Overload == Operator to Check Hash Values * * * * * * * * * * * * * * * //
int MESH::mesh_entity::operator==(const MESH::mesh_entity& subElement) const {

    // Check if element itself is sub-element
    if (this->seed == subElement.seed) {
        return 1;
    }
    else {
        return 0;
    }
}



/*------------------------------------------------------------------------*\
**  Class face Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //

MESH::face::face(mesh_entity e, bool boundaryFace)
:
    mesh_entity(e),
    _boundaryFace(boundaryFace)
{

}

MESH::face::face(int id, elementTypeEnum elementType, std::vector<std::weak_ptr<node>> nodes, bool subElement, bool boundaryFace)
:
    mesh_entity(id, elementType, nodes, subElement),
    _boundaryFace(boundaryFace)
{
    initialize();
}

// ONLY NODE IDs ARE PROVIDED, NEEDS TO BE INITILIAZED LATER
MESH::face::face(int id, elementTypeEnum elementType, std::vector<int> nodeIDs, bool subElement, bool boundaryFace)
:
    mesh_entity(id, elementType, nodeIDs, subElement),
    _boundaryFace(boundaryFace)
{

}


// * * * * * * * * * * * * * *  initialize * * * * * * * * * * * * * * * //
// Initializer of face class
void MESH::face::initialize() {

    // Calculate volume and centroid
    calculateVolume();
    calculateCentroid();

    // Get subelements
    hash();
}


// * * * * * * * * * * * * * *  get other element of face * * * * * * * * * * * * * * * //
std::shared_ptr<MESH::element> MESH::face::get_other_element(MESH::element e)
{
    assert( !_boundaryFace && "ERROR: Face is on the boundary, no neighbor element");
    std::vector<std::shared_ptr<element>> elements = get_elements();

    if ( e == *elements[0]) {
        return elements[1];
    }
    else {
        return elements[0];
    }
}


/*------------------------------------------------------------------------*\
**  Class element Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //
MESH::element::element(int id,elementTypeEnum elementType, std::vector<int> nodeIDs, bool subElement)
:
    mesh_entity(id,elementType,nodeIDs,subElement)
{

}

MESH::element::element(int id, elementTypeEnum elementType, std::vector<std::weak_ptr<node>> nodes, bool subElement)
:
    mesh_entity(id,elementType,nodes,subElement)
{
    // Run initialize method
    initializeInterior();
}

MESH::element::element(int id, elementTypeEnum elementType, std::vector<std::weak_ptr<face>> faces, bool subElement)
:
    mesh_entity(id,elementType,faces,subElement)
{
    // Assign faces to element
    _faces = faces;

    // Run initialize method
    initialize();
}


// * * * * * * * * * * * * * * Initialize * * * * * * * * * * * * * * * //
// Initializer of element class
void MESH::element::initialize() {
    initializeInterior();
    initializeExterior();
}

// * * * * * * * * * * * * * * Initialize * * * * * * * * * * * * * * * //
// Initializer of element class
void MESH::element::initializeInterior() {
    // Make sure faces are initialized
    assert(_nodes.size() != 0 && "element::initialize: nodes not initialized! Nodes must be set before element initialization!");

    // Calculate cell centroid 
    calculateCentroid();

    // Calculate cell volume
    calculateVolume();
    
    // Hashing for element comparison
    hash();
}

void MESH::element::initializeExterior() 
{
    assert(_faces.size() != 0 && "element::initialize: faces not initialized! Faces must be set before element initialization!");
    assert(_nodes.size() != 0 && "element::initialize: nodes not initialized! Nodes must be set before element initialization!");

    // Get face normals and distance weights
    calculateOutwardNormals();
    calculateFaceDistanceWeights();
}


// * * * * * * * * * * * * * * Add Face * * * * * * * * * * * * * * * //
bool MESH::element::add_face(std::weak_ptr<face> face) 
{
    auto it = std::find_if(_faces.begin(), _faces.end(), [&face](const std::weak_ptr<MESH::face>& f) {
        // Compare the shared_ptr held by the weak_ptr
        return f.lock() == face.lock();
    });

    if (it == _faces.end()) {
        _faces.push_back(face);
        return true;
    }
    return false;
}

// * * * * * * * * * * * * * *  determineSubElements * * * * * * * * * * * * * * * //
// return sub elements of a given element
std::vector<MESH::face> MESH::element::determineSubElements()
{
    std::vector<MESH::face> faces;
    std::vector<std::shared_ptr<node>> nodes = return_shared(&_nodes);

    // 1D ELEMENT IMPLEMENTATION
    if ( type2dimension[_elementType] == 1 ) faces = {};

    // 2D ELEMENT IMPLEMENTATION
    else if ( type2dimension[_elementType] == 2 ) {

        // First, get order of nodes from Jarvis March
        std::vector<MATH::Vector> coords;
        for (int i=0 ; i<_nodes.size() ; i++) {
            coords.push_back(nodes[i]->get_coordinates());
        }
        std::vector<int> order = MATH::jarvis_march(coords);

        // Now that we have our order, define our sub_elements
        for (int e=0; e<_nodes.size() ; e++) {
            // get list of nodes that make up element
            std::vector<std::weak_ptr<node>> nodes{ _nodes[order[e]] , _nodes[order[(e+1) % _nodes.size()]]}; 
            // Create sub element
            face subElement(-1, elementTypeEnum::LINE, nodes);
            faces.push_back(subElement);
        }
    }

    // 3D ELEMENT IMPLEMENTATION
    else if ( type2dimension[_elementType] == 3 ) {
        std::cerr << "ERROR: 3D elements not yet implemented. Element Type: " << _elementType << std::endl;
    }

    else {
        std::cerr << "ERROR: Element type not implemented. Element Type: " << _elementType << std::endl;
    }

    // 3D ELEMENTS NOT YET IMPLEMENTED

    return faces;
}



// * * * * * * * * * * * * * * Calculate Cell Normals * * * * * * * * * * * * * * * //
void MESH::element::calculateOutwardNormals()
{
    // Note: we need to use faces and not just nodes so indexes remain consistent

    std::vector<std::shared_ptr<face>> faces = return_shared(&_faces);

    std::vector<double> normal;
    for (int i=0 ; i<_faces.size() ; i++) {
        // 2D NORMAL FOR LINES
        if (faces[i]->get_type() == elementTypeEnum::LINE) {

            // Calculate unit normal
            double nx = faces[i]->get_nodes()[1]->get_coordinates()[1] - faces[i]->get_nodes()[0]->get_coordinates()[1];
            double ny = faces[i]->get_nodes()[1]->get_coordinates()[0] - faces[i]->get_nodes()[0]->get_coordinates()[0];
            double mag = std::sqrt(nx*nx + ny*ny);
            normal = {-nx/mag,ny/mag};

            // Ensure normal is pointing outward by taking dot product
            node centroid_diff = node(-1,_centroid - faces[i]->get_centroid());
            if ( (centroid_diff & node(-1,normal)) > 0.0) {
                normal = {-normal[0],-normal[1]};
            }
        }
        // 3D NORMAL FOR SURFACES
        else {
            std::cerr << "ERROR: 3D elements not yet implemented. Element Type: " << _elementType << std::endl;
        }

        // Store normal
        _normals.push_back(normal);
    }
}


// * * * * * * * * * * * * * * Face Distance Weight * * * * * * * * * * * * * * * //
void MESH::element::calculateFaceDistanceWeights()
{
    assert( _faces.size() != 0 && "ERROR: faces not initialized! Faces must be set before distance weight calculation!");
    std::vector<std::shared_ptr<face>> faces = get_faces();

    _distanceWeights = std::vector<double>(_faces.size());

    for (int i=0 ; i<_faces.size() ; i++) {
        // Calculate boundary distance weight
        if ( faces[i]->is_boundaryFace() ) 
        {
            _distanceWeights[i] = -1.0;
        }
        // Calculate interior distance weight
        else
        {
            std::shared_ptr<element> nb = get_neighbor(i);
            MATH::Vector v1 = MATH::Vector(_centroid - faces[i]->get_centroid());
            MATH::Vector v2 = MATH::Vector(nb->get_centroid() - faces[i]->get_centroid());
            double d1 = v1.getL2Norm();
            double d2 = v2.getL2Norm();
            
            // Get weighted distance between elements and face
            _distanceWeights[i] = (1.0/d1) / ((1.0/d1) + (1.0/d2)) ;
        }
        
        // // First check which sub-element (if any) elements share
        // for (std::shared_ptr<face> subelement : neighborElement.get_faces()) {
        //     int idx = *this == *subelement;
        //     if (idx != -1) {
        //         // Get distance between this element and face
        //         MATH::Vector v1 = MATH::Vector(_centroid - subelement->get_centroid());
        //         MATH::Vector v2 = MATH::Vector(neighborElement._centroid - subelement->get_centroid());
        //         double d1 = v1.getL2Norm();
        //         double d2 = v2.getL2Norm();
                
        //         // Get weighted distance between elements and face
        //         _distanceWeights[idx] = (1.0/d1) / ((1.0/d1) + (1.0/d2));
        //         neighborElement._distanceWeights[neighborElement == *subelement] = (1.0/d2) / ((1.0/d1) + (1.0/d2));
        //         return;
        //     }
        // }
    }
   
}

// * * * * * * * * * * * * * * Get Neighbor from index * * * * * * * * * * * * * * * //
std::shared_ptr<MESH::element> MESH::element::get_neighbor(unsigned int faceIdx)
{
    assert(faceIdx < _faces.size() && "ERROR: face index to get neighbor element out of bounds");
    
    std::vector<std::shared_ptr<face>> faces = get_faces();
    return faces[faceIdx]->get_other_element(*this);
}


// If this is a element, check all sub-elements
int MESH::element::operator==(const MESH::face& nface)
{
    std::vector<std::shared_ptr<face>> faces = return_shared(&_faces);
    
    for (int i=0 ; i<this->_faces.size() ; i++) {
        if (faces[i]->get_seed() == nface.get_seed()) return i;
    }
    return -1;
}


int MESH::element::operator==(const std::shared_ptr<MESH::face>& nface)
{
    std::vector<std::shared_ptr<face>> faces = return_shared(&_faces);
    
    for (int i=0 ; i<this->_faces.size() ; i++) {
        if (faces[i]->get_seed() == nface->get_seed()) return i;
    }
    return -1;
}

// Check equality to other element
bool MESH::element::operator==(const MESH::element& e) const
{
    return seed == e.get_seed();
}

// Check equality to other element
bool MESH::element::operator==(const std::shared_ptr<MESH::element>& e) const
{
    return seed == e->get_seed();
}





/*------------------------------------------------------------------------*\
**  Class node Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
MESH::node::node(int id, std::vector<double> coordinates) 
: 
    _id(id), 
    _coordinates(coordinates)
{}

MESH::node::node(int id, MATH::Vector coordinates) 
: 
    _id(id), 
    _coordinates(coordinates)
{}


// * * * * * * * * * * * * * *  Distance Weights * * * * * * * * * * * * * * * //
void MESH::node::calculateElementDistanceWeights()
{
    assert(_elements.size() > 0 && "ERROR: Node has no elements");
    assert(_distanceWeights.size() == 0 && "ERROR: Node already has distance weights");

    std::vector<std::shared_ptr<element>> elements = return_shared(&_elements);

    std::vector<double> distances;
    double total = 0.0;
    MATH::Vector dist;

    for (int i=0 ; i<_elements.size() ; i++) {
        dist = (elements)[i]->get_centroid() - _coordinates;
        distances.push_back(dist.getL2Norm());
        total += 1.0/dist.getL2Norm();
    }

    for (int i=0 ; i<_elements.size() ; i++) {
        _distanceWeights.push_back((1.0/distances[i])/total);
    }

}

// * * * * * * * * * * * * * *  operator- * * * * * * * * * * * * * * * //
// Overloaded - operator: return the vector distance between two nodes
MATH::Vector MESH::node::operator-(const node& obj) {
    MATH::Vector diff = _coordinates - obj._coordinates;
    return diff;
}

// * * * * * * * * * * * * * *  operator* * * * * * * * * * * * * * * * //
// Overloaded * operator: return the distance between two nodes
double MESH::node::operator*(const node &obj) const {
    double sum = 0.0;
    for (int i=0 ; i<_coordinates.size() ; i++ ) {
        sum += pow(_coordinates[i] - obj._coordinates[i], 2.0);
    }
    return std::sqrt(sum);
}

// * * * * * * * * * * * * * *  operator/ * * * * * * * * * * * * * * * //
// Overloaded / operator: return the cross product of two nodes
MATH::Vector MESH::node::operator/(const node &obj) const {
    assert(     (_coordinates.size() == 3 || _coordinates.size() == 2)
            && (obj._coordinates.size() == 3 || _coordinates.size() == 2) 
            && "Coordinates must be dimension 2 or 3 to take cross product");

    MATH::Vector cross(_coordinates.size());
    cross[0] = _coordinates[1] * obj._coordinates[2] - _coordinates[2] * obj._coordinates[1];
    cross[1] = _coordinates[2] * obj._coordinates[0] - _coordinates[0] * obj._coordinates[2];
    cross[2] = _coordinates[0] * obj._coordinates[1] - _coordinates[1] * obj._coordinates[0];
    return cross;
}

// * * * * * * * * * * * * * *  operator& * * * * * * * * * * * * * * * //
// Overloaded & operator: return the dot product of two nodes
double MESH::node::operator&(const node &obj) const {
    double dot=0.0;
    for (int i=0 ; i<_coordinates.size() ; i++ ) {
        dot += _coordinates[i] * obj._coordinates[i];
    }
    return dot;
}

/*------------------------------------------------------------------------*\
**  Class Boundary Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
MESH::Boundary::Boundary(std::string name, std::vector<std::weak_ptr<face>> faces, int id)
:
    _name(name),
    _faces(faces),
    _bcID(id)
{}

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
MESH::Boundary::Boundary(std::string name, std::vector<std::string> faceSeeds, int id)
:
    _name(name),
    _faceSeeds(faceSeeds),
    _bcID(id)
{}

// * * * * * * * * * * * * * * Add face to boundary * * * * * * * * * * * * * * * //
bool MESH::Boundary::add_face(std::weak_ptr<face> face)
{
    // lambda function for comparing faces
    auto it = std::find_if(_faces.begin(), _faces.end(), [&face](const std::weak_ptr<MESH::face>& f) {
        // Compare the shared_ptr held by the weak_ptr
        return f.lock() == face.lock();
    });

    if (it == _faces.end()) {
        _global2local[face.lock()->get_id()] = _faces.size()-1;
        _faces.push_back(face);
        return true;
    }
    return false;
}

// * * * * * * * * * * * * * *  Check if global face index is on boundary * * * * * * * * * * * * * * * //
bool MESH::Boundary::onBoundary(int globalID) const
{
    return _global2local.find(globalID) != _global2local.end();
}