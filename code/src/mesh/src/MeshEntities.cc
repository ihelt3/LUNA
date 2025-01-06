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

MESH::mesh_entity::mesh_entity(int id, elementTypeEnum elementType, std::vector<node> nodes, bool subElement)
:
    _id(id),
    _elementType(elementType),
    _nodes(nodes),
    _subElement(subElement),
    _centroid({})
{
    // Define node IDs from node vector
    for (int i=0 ; i<_nodes.size() ; i++) {
        _nodeIDs.push_back(_nodes[i].get_id());
    }

    // Run initialize method
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

// * * * * * * * * * * * * * *  calculateVolume * * * * * * * * * * * * * * * //
void MESH::mesh_entity::calculateVolume() 
{
    // LINE Volume = Length
    if (_elementType == elementTypeEnum::LINE) {
        _volume = _nodes[0]*_nodes[1];
    }
    // TRIANGLE Volume = Area
    if (_elementType == elementTypeEnum::TRIANGLE) {
        _volume = triArea(_nodes);
    }
    // QUADRILATERAL Volume = Area
    if (_elementType == elementTypeEnum::QUADRILATERAL) {
        std::vector<std::vector<node>> diags = returnQuadDiags(_nodes);
        // Generalized area of a quadrilateral is calculating the area of the two triangles that make it
        _volume = triArea({diags[0][0],diags[0][1],diags[1][0]}) + triArea({diags[0][0],diags[0][1],diags[1][1]});
    }
}

// * * * * * * * * * * * * * * Calculate Cell Centroid * * * * * * * * * * * * * * * //
void MESH::mesh_entity::calculateCentroid() 
{
    MATH::Vector center(_nodes[0].get_coordinates().size(),0.0);
    double c = _nodes.size();
    // Just add all the nodes index wise and divide by number of nodes
    for (int i=0 ; i<_nodes.size() ; i++) {
        center = center + _nodes[i].get_coordinates();
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
{}

MESH::face::face(int id, elementTypeEnum elementType, std::vector<node> nodes, bool subElement, bool boundaryFace)
:
    mesh_entity(id, elementType, nodes, subElement),
    _boundaryFace(boundaryFace)
{
    initialize();
}

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


/*------------------------------------------------------------------------*\
**  Class element Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //
MESH::element::element(int id,elementTypeEnum elementType, std::vector<int> nodeIDs, bool subElement)
:
    mesh_entity(id,elementType,nodeIDs,subElement)
{

}

MESH::element::element(int id, elementTypeEnum elementType, std::vector<node> nodes, bool subElement)
:
    mesh_entity(id,elementType,nodes,subElement)
{
    // Run initialize method
    initialize();
}


// * * * * * * * * * * * * * *  initialize * * * * * * * * * * * * * * * //
// Initializer of element class
void MESH::element::initialize() {

    // Calculate volume and centroid
    calculateVolume();
    calculateCentroid();

    // Get subelements
    determineSubElements();
    calculateOutwardNormals();
    
    hash();
}

// * * * * * * * * * * * * * *  determineSubElements * * * * * * * * * * * * * * * //
// return sub elements of a given element
void MESH::element::determineSubElements()
{
    // LINE has no sub elements
    if (_elementType == elementTypeEnum::LINE) _faces = {};
    // TRIANGLE has 3 LINE subelements
    else if (_elementType == elementTypeEnum::TRIANGLE) {
        for (int e=0; e<3 ; e++) {
            // get list of nodes that make up element
            std::vector<node> nodes{_nodes[e],_nodes[(e+1) % 3]}; 
            // Create sub element
            face subElement(e, elementTypeEnum::LINE, nodes);
            _faces.push_back(subElement);
        }
    }
    // QUADRILATERAL has 4 LINE subelements
    else if (_elementType == elementTypeEnum::QUADRILATERAL) {
        std::vector<std::vector<node>> diags = returnQuadDiags(_nodes);

        // Now that we have our cross nodes, define our elements
        std::vector<node> line1{diags[0][0],diags[1][0]};
        std::vector<node> line2{diags[0][1],diags[1][1]};
        std::vector<node> line3{diags[0][0],diags[1][1]};
        std::vector<node> line4{diags[0][1],diags[1][0]};

        face subLine1(-1,elementTypeEnum::LINE,line1);
        face subLine2(-1,elementTypeEnum::LINE,line2);
        face subLine3(-1,elementTypeEnum::LINE,line3);
        face subLine4(-1,elementTypeEnum::LINE,line4);
        _faces = {subLine1,subLine2,subLine3,subLine4};
    }
    else {
        std::cerr << "ERROR: 3D elements not yet implemented. Element Type: " << _elementType << std::endl;
    }

    // resize distance weighted vector to size of subelements
    _distanceWeights.resize(_faces.size(),-1.0);

    // 3D ELEMENTS NOT YET IMPLEMENTED
}



// * * * * * * * * * * * * * * Calculate Cell Normals * * * * * * * * * * * * * * * //
void MESH::element::calculateOutwardNormals()
{
    std::vector<double> normal;
    for (int i=0 ; i<_faces.size() ; i++) {
        // 2D NORMAL FOR LINES
        if (_faces[i].get_type() == elementTypeEnum::LINE) {

            // Calculate unit normal
            double nx = _faces[i].get_nodes()[1].get_coordinates()[1] - _faces[i].get_nodes()[0].get_coordinates()[1];
            double ny = _faces[i].get_nodes()[1].get_coordinates()[0] - _faces[i].get_nodes()[0].get_coordinates()[0];
            double mag = std::sqrt(nx*nx + ny*ny);
            normal = {-nx/mag,ny/mag};

            // Ensure normal is pointing outward by taking dot product
            node centroid_diff = node(-1,_centroid - _faces[i].get_centroid());
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
        _faces[i].set_normal(normal);
    }
    
    
}


// * * * * * * * * * * * * * * Face Distance Weight * * * * * * * * * * * * * * * //
void MESH::element::calculateFaceDistanceWeight(MESH::element &neighborElement)
{
    // First check which sub-element (if any) elements share
    for (face& subelement : neighborElement._faces) {
        int idx = *this == subelement;
        if (idx != -1) {
            // Get distance between this element and face
            MATH::Vector v1 = MATH::Vector(_centroid - subelement.get_centroid());
            MATH::Vector v2 = MATH::Vector(neighborElement._centroid - subelement.get_centroid());
            double d1 = v1.getL2Norm();
            double d2 = v2.getL2Norm();
            
            // Get weighted distance between elements and face
            _distanceWeights[idx] = (1.0/d1) / ((1.0/d1) + (1.0/d2));
            neighborElement._distanceWeights[neighborElement == subelement] = (1.0/d2) / ((1.0/d1) + (1.0/d2));
            return;
        }
    }
   
}

// If this is a element, check all sub-elements
int MESH::element::operator==(const MESH::face& element) const
{
    for (int i=0 ; i<this->_faces.size() ; i++) {
        if (this->_faces[i].get_seed() == element.get_seed()) return i;
    }
    return -1;
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
void MESH::node::calculateElementDistanceWeights(const std::vector<element>* elements)
{
    assert(_elements.size() > 0 && "ERROR: Node has no elements");
    assert(_distanceWeights.size() == 0 && "ERROR: Node already has distance weights");

    std::vector<double> distances;
    double total = 0.0;
    MATH::Vector dist;

    for (int i=0 ; i<_elements.size() ; i++) {
        dist = (*elements)[i].get_centroid() - _coordinates;
        distances.push_back(dist.getL2Norm());
        total += 1.0/dist.getL2Norm();
    }

    for (int i=0 ; i<_elements.size() ; i++) {
        _distanceWeights.push_back((1.0/distances[i])/total);
    }

}

// * * * * * * * * * * * * * *  operator- * * * * * * * * * * * * * * * //
// Overloaded - operator: return the difference between two nodes
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
MESH::Boundary::Boundary(std::string name, std::vector<face> faces, int id)
:
    _name(name),
    _faces(faces),
    _bcID(id)
{
    // Initialize face mapping vectors
    _local2global.resize(_faces.size());
}

// * * * * * * * * * * * * * *  Check if global face index is on boundary * * * * * * * * * * * * * * * //
bool MESH::Boundary::onBoundary(int globalID) const
{
    return _global2local.find(globalID) != _global2local.end();
}

// Get global face index of local face index
int MESH::Boundary::get_globalID(int localID) const
{ 
    // Check if localID is in boundary
    assert(localID >= 0 && localID < _faces.size() && "ERROR: localID is not in boundary");
    return _local2global[localID]; 
}

int MESH::Boundary::get_localID(int globalID) const
{
    // Check if globalID is in boundary
    std::ostringstream errorMsg;
    errorMsg << "ERROR: global face ID " << globalID << " is not in boundary";
    assert( onBoundary(globalID) && errorMsg.str().c_str() );
    return _local2global[globalID];
}