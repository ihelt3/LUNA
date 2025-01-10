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
        // Calculate distance weights to elements
        void calculateElementDistanceWeights(const std::vector<element>*);
        // Check if boundary node
        bool is_boundaryNode() const { return _onBoundary; };

    // get methods
        int get_id() const { return _id; };
        MATH::Vector get_coordinates() const { return _coordinates; };
        std::vector<int> get_elements() const { return _elements; };
        std::vector<int> get_faces() const { return _faces; };
        std::vector<double> get_distanceWeights() const { return _distanceWeights; };

    // set methods
        void set_elements(std::vector<int> elements) { _elements = elements; };
        void add_element(int element) { _elements.push_back(element); };
        void set_faces(std::vector<int> faces) { _faces = faces; };
        void add_face(int face) { _faces.push_back(face); };
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
        std::vector<int> _elements{};
        // Faces 
        std::vector<int> _faces{};
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
        mesh_entity(int, elementTypeEnum, std::vector<int>, bool subElement=false);
        // instantiate from vector of nodes
        mesh_entity(int, elementTypeEnum, std::vector<node>, bool subElement=false);
    
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
        void set_nodes(std::vector<node> nodes) { _nodes = nodes; };
        void set_id(int id) { _id = id; };

    // Get Methods
        const int& get_id() const { return _id; };
        const elementTypeEnum& get_type() const { return _elementType; };
        const std::vector<node>& get_nodes() const { return _nodes; };
        const double& get_volume() const { return _volume; };
        const std::vector<int>& get_nodeIDs() const { return _nodeIDs; };
        const MATH::Vector& get_centroid() const { return _centroid; };
        const std::string& get_seed() const { return seed; };

    // Operator Overloading
        // Check if cell contains sub-element
        int operator==(const mesh_entity& other) const;

protected:
    // Member data
        // Element id
        int _id;
        // Element type
        elementTypeEnum _elementType;
        // node vector
        std::vector<node> _nodes;
        // cell centroid
        MATH::Vector _centroid;
        // true/false sub-element
        bool _subElement;
        // vector of node-ids
        std::vector<int> _nodeIDs;
        // Cell "volume"
        double _volume;                 // 1D: width,       2D: area,   3D: volume
        // hash object for element comparisons
        std::string seed;
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
        face(mesh_entity, bool boundaryFace=false);
        // instantiate from vector of node IDs
        face(int, elementTypeEnum, std::vector<int>, bool subElement=true, bool boundaryFace=false);
        // instantiate from vector of nodes
        face(int, elementTypeEnum, std::vector<node>, bool subElement=true, bool boundaryFace=false);

    // Member Functions
        // Initialize element
        void initialize() override; 

    // Set methods
        void set_elements(std::vector<int> elements) { _elements = elements; };
        void add_element(int element) { _elements.push_back(element); };
        void set_boundaryFace(bool bface) { _boundaryFace = bface; };
        void set_normal(MATH::Vector normal) { _normal = normal; };

    // Get methods
        const std::vector<int>& get_elements() const { return _elements; };
        const bool& is_boundaryFace() const { return _boundaryFace; };
        const MATH::Vector& get_normal() const { return _normal; };

private:
    // Private member data
        // Elements attached to this faces
        std::vector<int> _elements {};
        // If face is a boundary face
        bool _boundaryFace;
        // Normal corresponds to first element's outward facing normal
        MATH::Vector _normal;
};


/*------------------------------------------------------------------------*\
**  Class element Declaration
\*------------------------------------------------------------------------*/

// generic element which is either made up of nodes (line) [1D], lines (tri,quad) [2D], or combinations of tris and quads [3D]
class element
:
    public mesh_entity
{
public:
    // Constructor
        // Default constructor: invalid element
        element() : mesh_entity() {};
        // instantiate from vector of node IDs
        element(int, elementTypeEnum, std::vector<int>, bool subElement=false);
        // instantiate from vector of nodes
        element(int, elementTypeEnum, std::vector<node>, bool subElement=false);

    // Member Functions
        // initialize element
        void initialize() override;
        // get sub elements of element
        void determineSubElements();
        // Calculate element outward facing normal (for sub-element)
        void calculateOutwardNormals();
        // Calculate face distance weight
        void calculateFaceDistanceWeight(element&);

    // set methods
        void set_subElement(bool subElement) { _subElement = subElement; };
        void set_face_id(int faceIdx, int faceID) { _faces[faceIdx].set_id(faceID); };
        void set_face_connection(int faceIdx, int neighborID) { _faces[faceIdx].set_elements({neighborID}); };
        void set_boundaryFace(int faceIdx) { _faces[faceIdx].set_boundaryFace(true); };

    // get methods ( [const type& get() const {}] returns a const reference, i.e. reference to data to avoid copying data)
        const std::vector<face>& get_faces() const { return _faces; };
        const std::vector<MATH::Vector>& get_normals() const { return _normals; };
        const std::vector<double>& get_distanceWeights() const { return _distanceWeights; };

    // Operator Overloading
        // Override == operator from base class to check for subelements
        int operator==(const face&) const;

protected:
    // Member Data
        // sub-elements
        // NOTE: element faces only have one element ID, which is the ID of the neighbor element
        std::vector<face> _faces;
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
        // Construct from name and element vector
        Boundary(std::string,std::vector<face>,int);

    // Public member data

    // Member functions
        void map_global_and_local(int global, int local) { _global2local[global] = local; _local2global[local] = global;};
        int get_globalID(int localID) const;
        int get_localID(int globalID) const;
        // Check if global face index is on boundary
        bool onBoundary(int globalID) const;

    // get methods
        // Get boundary faces
        std::vector<face>& get_faces() { return _faces; };
        const std::vector<face>& get_faces() const { return _faces; };
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
        std::vector<face> _faces;
        // Boundary condition id
        int _bcID;
        // Mapping global face indexes and local face indexes
        std::unordered_map<int, int> _global2local;
        std::vector<int> _local2global;
};

}

#endif // _MESHENTITIES_HH_