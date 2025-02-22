/*------------------------------------------------------------------------*\
**  
**  @file:      MeshEntities.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     mesh entities definitions
**
\*------------------------------------------------------------------------*/

#ifndef _MESHENTITIES_HH_
#define _MESHENTITIES_HH_

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "Vector.hh"
#include "topology.hh"

// Define type enum for element/cell types
enum elementTypeEnum{
    INVALID,
    LINE,
    TRIANGLE,
    QUADRILATERAL,
    TETRAHEDRAL,
    HEXAHEDRAL,
    PRISM,
    PYRAMID
};

// Map Element Type to dimension (external linkage)
inline std::unordered_map<elementTypeEnum, int> type2dimension = {
    {elementTypeEnum::LINE, 1},
    {elementTypeEnum::TRIANGLE, 2},
    {elementTypeEnum::QUADRILATERAL, 2},
    {elementTypeEnum::TETRAHEDRAL, 3},
    {elementTypeEnum::HEXAHEDRAL, 3},
    {elementTypeEnum::PRISM, 3},
    {elementTypeEnum::PYRAMID, 3}
};

// Enum for each element type
enum class numberOfNodes {
    INVALID=0,
    LINE=2, 
    TRIANGLE=3, 
    QUADRILATERAL=4, 
    TETRAHEDRAL=4, 
    HEXAHEDRAL=8, 
    PRISM=6, 
    PYRAMID=5
};

// Forward declarations
class element;
class node;
class cell;
class Boundary;

namespace MESH {

// Forward Declarations
class node;
class element;
class face;
class Boundary;


/*------------------------------------------------------------------------*\
**  Supporting function to return a vector of locked pointers
\*------------------------------------------------------------------------*/
template<typename T>
std::vector<std::shared_ptr<T>> return_shared(std::vector<std::weak_ptr<T>>* weakPtrs) 
{
    if (!weakPtrs) return {}; // Handle nullptr input safely

    // Initialize vector of shared pointers
    std::vector<std::shared_ptr<T>> sharedPtrs;
    sharedPtrs.reserve(weakPtrs->size());

    // Convert weak_ptrs to shared_ptrs, filtering out expired ones
    weakPtrs->erase(std::remove_if(weakPtrs->begin(), weakPtrs->end(),
        [&](std::weak_ptr<T>& weakPtr) {
            if (auto sharedPtr = weakPtr.lock()) {
                sharedPtrs.push_back(sharedPtr);
                return false; // Keep valid weak_ptrs
            }
            return true; // Remove expired weak_ptrs
        }),
        weakPtrs->end());

    return sharedPtrs;
}

template<typename T>
std::shared_ptr<T> return_shared(std::weak_ptr<T>* weakPtr)
{
    if (!weakPtr) return nullptr; // Handle nullptr input safely

    if (auto sharedPtr = weakPtr->lock()) {
        return sharedPtr;
    }
    return nullptr;
}

template<typename T>
std::vector<std::shared_ptr<T>> return_shared(std::vector<T>* nonPtr)
{
    std::vector<std::shared_ptr<T>> sharedPtrs;
    for (auto& ptr : *nonPtr) {
        sharedPtrs.push_back(std::make_shared<T>(ptr));
    }
    return sharedPtrs;
}

/*------------------------------------------------------------------------*\
**  Class node Declaration
\*------------------------------------------------------------------------*/

class node
{
public:
    // Constructor
        // Construct with double vector
        node(int, std::vector<double>);
        // Construct with Vector class
        node(int, MATH::Vector);

    
    // Public member data

    // Member Functions
        // Calculate distance weights to elements, using vector of all elements in mesh
        void calculateElementDistanceWeights();
        // Check if boundary node
        bool is_boundaryNode() const { return _onBoundary; };

    // get methods
        int get_id() const { return _id; };
        MATH::Vector get_coordinates() const { return _coordinates; };
        std::vector<std::shared_ptr<element>> get_elements() { return return_shared(&_elements); };
        std::vector<std::shared_ptr<face>> get_faces() { return return_shared(&_faces); };
        std::vector<double> get_distanceWeights() const { return _distanceWeights; };

    // set methods
        void set_elements(std::vector<std::weak_ptr<element>> elements) { _elements = elements; };
        void add_element(std::weak_ptr<element> element) { _elements.push_back(element); };
        void set_faces(std::vector<std::weak_ptr<face>> faces) { _faces = faces; };
        void add_face(std::weak_ptr<face> face) { _faces.push_back(face); };
        void set_boundary(bool onBoundary) { _onBoundary = onBoundary; };

    // Operator Overloading
        // Calculate the difference in position between two nodes
        MATH::Vector operator-(const node&);
        // Calculate distance between two nodes
        double operator*(const node&) const;
        // Calculate cross product of two nodes
        MATH::Vector operator/(const node&) const;
        // Calculate dot product of two nodes (& consistent with openFOAM)
        double operator&(const node&) const;
    

private:
    // Private member data
        // Node ID
        int _id;
        // Node coordinates
        MATH::Vector _coordinates;
        // Elements
        std::vector<std::weak_ptr<element>> _elements{};
        // Faces 
        std::vector<std::weak_ptr<face>> _faces{};
        // distance weight to elements
        std::vector<double> _distanceWeights;
        // distance weight to faces
        // Check if on boundary
        bool _onBoundary = false;
};


/*------------------------------------------------------------------------*\
**  Class mesh entity (base class for elements and faces)
\*------------------------------------------------------------------------*/
class mesh_entity
{
public:
    // Constructors
        // Default constructor
        mesh_entity() : _id(-1), _elementType(elementTypeEnum::INVALID), _nodes({}), _centroid({}) {};
        // instantiate from vector of node IDs
        mesh_entity(int, elementTypeEnum, std::vector<int>);
        // instantiate from vector of nodes
        mesh_entity(int, elementTypeEnum, std::vector<std::weak_ptr<node>>);
        // instantiate from vector of faces
        mesh_entity(int, elementTypeEnum, std::vector<std::weak_ptr<face>>);
    
    // Member Functions
        // initialize mesh entity
        virtual void initialize();
        // Calculate mesh volume
        void calculateVolume();
        // Calculate centroid
        void calculateCentroid();
        // General method to calculate area of triangle
        double triArea(std::vector<node>);
        // Return diagonals of a quad element
        std::vector<std::vector<node>> returnQuadDiags(std::vector<node>);
        // Hashing function for quick subelement comparisons
        void hash();

    // Set Methods
        void set_nodes(std::vector<std::weak_ptr<node>> nodes) { _nodes = nodes; };
        void set_id(int id) { _id = id; };

    // Get Methods
        const int& get_id() const { return _id; };
        const elementTypeEnum& get_type() const { return _elementType; };
        const std::vector<std::shared_ptr<node>> get_nodes() { return return_shared(&_nodes); };
        const double& get_volume() const { return _volume; };
        const std::vector<int>& get_nodeIDs() const { return _nodeIDs; };
        const MATH::Vector& get_centroid() const { return _centroid; };
        const std::string& get_seed() const { return seed; };

    // Operator Overloading
        // Check if cell contains sub-element
        int operator==(const mesh_entity&) const;

protected:
    // Member data
        // Element id
        int _id;
        // Element type
        elementTypeEnum _elementType;
        // node vector
        std::vector<std::weak_ptr<node>> _nodes;
        // cell centroid
        MATH::Vector _centroid;
        // vector of node-ids
        std::vector<int> _nodeIDs;
        // Cell "volume"
        double _volume;                 // 1D: width,       2D: area,   3D: volume
        // hash object for element comparisons
        std::string seed;

    // 
};



/*------------------------------------------------------------------------*\
**  Class face Declaration
\*------------------------------------------------------------------------*/
// face class: interface between cells, inherits element class
class face 
: 
    public mesh_entity
{
public:
    // Constructor
        // Default constructor: invalid face
        face() : mesh_entity() {};
        // Copy from element class
        explicit face(mesh_entity, bool boundaryFace=false);
        // instantiate from vector of node IDs
        face(int, elementTypeEnum, std::vector<int>, bool boundaryFace=false);
        // instantiate from vector of nodes
        face(int, elementTypeEnum, std::vector<std::weak_ptr<node>>, bool boundaryFace=false);

    // Member Functions
        // Initialize element
        void initialize() override; 
        // Calculate face normal
        void calculateNormal();

    // Set methods
        void set_owner(std::weak_ptr<element> owner);
        void set_neighbor(std::weak_ptr<element> neighbor);
        void set_boundary(int boundaryID) { _boundaryID = boundaryID; _boundaryFace = true; };

    // Get methods
        // Get owner element
        const std::shared_ptr<element> get_owner() { return return_shared(&_owner); };
        // Get neighbor element
        const std::shared_ptr<element> get_neighbor() { return return_shared(&_neighbor); };
        // Get face normal
        const MATH::Vector& get_normal() const { return _normal; };
        // Get both elements
        std::vector<std::shared_ptr<element>> get_elements();
        // Get element that is not the given element
        std::shared_ptr<element> get_other_element(element);
        // Check if this is boundary face
        const bool& is_boundaryFace() const { return _boundaryFace; };
        // Get boundary ID
        const int& get_boundaryID() const { return _boundaryID; };

private:
    // Private member data
        // Owner element
        std::weak_ptr<element> _owner;
        // Neighbor element
        std::weak_ptr<element> _neighbor;
        // Face normal (outward pointwing w.r.t owner)
        MATH::Vector _normal;
        // If face is a boundary face (default to no boundary)
        bool _boundaryFace = false;
        int _boundaryID;
};


/*------------------------------------------------------------------------*\
**  Class element Declaration
\*------------------------------------------------------------------------*/

// generic element which is either made up of nodes (line) [1D], lines (tri,quad) [2D], or combinations of tris and quads [3D]
class element
:
    public mesh_entity,
    public std::enable_shared_from_this<element>
{
public:
    // Constructor
        // Default constructor: invalid element
        element() : mesh_entity() {};
        // instantiate from vector of node IDs
        element(int, elementTypeEnum, std::vector<int>);
        // instantiate from vector of nodes
        element(int, elementTypeEnum, std::vector<std::weak_ptr<node>>);
        // instantiate from vector of faces
        element(int, elementTypeEnum, std::vector<std::weak_ptr<face>>);

    // Member Functions
        // initialize element
        void initialize() override;
        // Initialize element interior
        void initializeInterior();
        // Initialize element exterior
        void initializeExterior();
        
        // get sub elements of element
        std::vector<face> determineSubElements();
        // Calculate element outward facing normal (for sub-element)
        void calculateOutwardNormals();
        // Calculate face distance weight
        void calculateFaceDistanceWeights();
        // Add face to element
        bool add_face(std::weak_ptr<face> face);
        // Get element neighbor from LOCAL face index
        std::shared_ptr<element> get_neighbor(unsigned int faceIdx);

    // set methods

    // get methods ( [const type& get() const {}] returns a const reference, i.e. reference to data to avoid copying data)
        const std::vector<std::shared_ptr<face>> get_faces() { return return_shared(&_faces); };
        const std::vector<MATH::Vector>& get_normals() const { return _normals; };
        const std::vector<double>& get_distanceWeights() const { return _distanceWeights; };

    // Operator Overloading
        // Override == operator from base class to check for subelements
        int operator==(const face&);
        int operator==(const std::shared_ptr<face>& nface);
        // Check if two elements are the same
        bool operator==(const element&) const;
        bool operator==(const std::shared_ptr<element>&) const;

protected:
    // Member Data
        // sub-elements
        // NOTE: element faces only have one element ID, which is the ID of the neighbor element
        std::vector<std::weak_ptr<face>> _faces;
        // distance weight to sub-elements, indexed by faces
        std::vector<double> _distanceWeights;
        // Outward facing normals
        std::vector<MATH::Vector> _normals;
        
};





/*------------------------------------------------------------------------*\
**  Class Boundary Declaration
\*------------------------------------------------------------------------*/

class Boundary
{
public:
    // Constructors
        // Construct from name, element vector and ID
        Boundary(std::string,std::vector<std::weak_ptr<face>>,int);
        // Construct from name, face seed vector and ID
        Boundary(std::string,std::vector<std::string>,int);

    // Public member data

    // Member functions
        void map_global2local(int global, int local) { _global2local[global] = local; };
        bool add_face(std::weak_ptr<face> face);
        bool onBoundary(int) const;

    // get methods
        // Get pointers to boundary faces
        const std::vector<std::shared_ptr<face>> get_faces() { return return_shared(&_faces); };
        // Get face seed vector
        const std::vector<std::string>& get_faceSeeds() const { return _faceSeeds; };
        // Get boundary name
        const std::string get_name() const { return _name; };
        // Get boundary ID
        const int& get_id() const { return _bcID; };

    // set methods


protected:
    // Private member data
        // Name of boundary condition
        const std::string _name;
        // Boundary condition elements
        std::vector<std::weak_ptr<face>> _faces;
        // Face seed vector for temporary face creation
        std::vector<std::string> _faceSeeds;
        // Boundary condition id
        int _bcID;
        // Mapping global face indexes and local face indexes
        std::unordered_map<int, int> _global2local;
};

}

#endif // _MESHENTITIES_HH_