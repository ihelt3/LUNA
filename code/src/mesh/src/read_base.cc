/*------------------------------------------------------------------------*\
**  
**  @file:      read_base.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for read mesh base class
**
\*------------------------------------------------------------------------*/

#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <sstream>
#include <cassert>
#include <stdexcept>

#include "read_base.hh"
#include "mesh.hh"

/*------------------------------------------------------------------------*\
**  Class read_base Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
MESH::read_base::read_base(std::filesystem::path filePath, bool verbose ) 
:
    _verbose(verbose)
{
    // Open the file
    if (_verbose) std::cout << "filepath: " << std::filesystem::absolute(filePath) << " " << filePath << std::endl;
    _file.open(filePath);
    if (!_file.is_open())
    {
        std::cerr << "Cannot open mesh file for reading!";
        exit(1);
    }

}

// * * * * * * * * * * * * * *  Destructor * * * * * * * * * * * * * * * //
MESH::read_base::~read_base() 
{
    _file.close();
}


// * * * * * * * * * * * * * *  get_mesh * * * * * * * * * * * * * * * //
MESH::mesh MESH::read_base::get_mesh() {
    // Assert variables are initialized
    assert(Mesh._dimension > 1);
    assert(!Mesh._elements.empty());
    assert(!Mesh._nodes.empty());
    assert(!Mesh._boundaries.empty());
    assert(!Mesh._faceNormalDeltas.empty());

    // Return mesh
    if (_verbose) std::cout << "Mesh statistics: " << std::endl;
    if (_verbose) std::cout << Mesh << std::endl;
    return Mesh;
}


// * * * * * * * * * * * * * *  check_in_file * * * * * * * * * * * * * * * //
// Check for string in file
bool MESH::read_base::check_in_file(const std::string str) {
    std::string line; 
    int offset;

    assert(_file.is_open() == true && "The mesh file should be open for reading");
    while (!_file.eof()) {
        getline(_file,line);
        if ((offset = line.find(str,0)) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

// * * * * * * * * * * * * * *  get_line_of_str * * * * * * * * * * * * * * * //
// Return line with specific string
std::string MESH::read_base::get_line_of_str(const std::string str, const::std::string comment) {
    std::string line; 
    int offset;

    assert(_file.is_open() == true && "The mesh file should be open for reading");
    while (!_file.eof()) {
        // Get line and ignore any comments
        getline(_file,line);

        if (comment != "") {
            line = line.substr(0,line.find(comment));
        }

        if ((offset = line.find(str,0)) != std::string::npos)
        {
            return line;
        }
    }
    std::cerr << "Error: Could not find string in file: " << str;
    return "";
}

// * * * * * * * * * * * * * *  instantiateElements * * * * * * * * * * * * * * * //
// Instantiate any elements that were constructed with only node ids and add faces
/*
** NOTE: Assumes _nodes vector is already initialized
**       Assumes _elements vector is initialized, but does not require _elements._faces to be defined
**       Assumes _faces is NOT initialized
**       Assumes _BCs is initialized
**
**       Essentially we are only given the nodes that make up everything, and have already initialized elements and BCs with that info
**       We just need to add the faces to everything
*/       
void MESH::read_base::instantiateElements()
{
    auto start = std::chrono::steady_clock::now();

    // Some particular assert statements...
    assert(Mesh._nodes.size() > 0 && "read_base::instantiateElements: Requires nodes vector to be initialized");
    assert(Mesh._elements.size() > 0 && "read_base::instantiateElements: Requires elements vector to be initialized");
    assert(Mesh._faces.size() == 0 && "read_base::instantiateElements: Requires faces vector to be un-initialized");
    assert(Mesh._boundaries.size() > 0 && "read_base::instantiateElements: Requires BCs vector to be initialized");


    // Map of face hashes
    std::unordered_map<std::string, std::shared_ptr<face>> faceMap;
    // Map for face order
    std::unordered_map<int, std::string> faceOrder;
    // Map element ID to face IDs
    std::unordered_map<int, std::vector<int>> elementFaceMap;
    // Tracker for faceIDs
    int faceID = 0;

    //=================================================================================================
    // Loop through elements 
    for (int c=0 ; c<Mesh._elements.size() ; c++)
    {
        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
        // Assign nodes to element
        if (Mesh._elements[c]->get_nodes().size() == 0) {

            // Nodes have not been assigned: get nodes from IDs and initialize element
            std::vector<int> ids = Mesh._elements[c]->get_nodeIDs();
            std::vector<std::weak_ptr<node>> nodeList;
            for (int n=0 ; n<ids.size() ; n++) {
                nodeList.push_back( Mesh._nodes[ids[n]] );
            }

            // Reinitializing element with node vector
            Mesh._elements[c]->set_nodes(nodeList);
            Mesh._elements[c]->initializeInterior();
        }

        // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
        // Loop through element faces and add to faceMap
        if (Mesh._elements[c]->get_faces().size() == 0) {

            // Faces have not been assigned, make faces
            // NOTE: face owner is defined in determineSubElements
            std::vector<face> faces = Mesh._elements[c]->determineSubElements();

            // Check if faces exist in map
            for (face& f : faces) {
                // First instance of face
                if ( faceMap.find(f.get_seed()) == faceMap.end() ) {
                    // Assign face order
                    faceOrder[faceID] = f.get_seed();
                    f.set_id(faceID);
                    // Add face to faceMap (and add element to face)
                    faceMap[f.get_seed()] = std::make_shared<face>(f);
                    // Increment id for new element
                    faceID++;
                }
                // Second instance of face
                else
                {
                    // Map element to face
                    faceMap[f.get_seed()]->set_neighbor( Mesh._elements[c] );
                }
            }
        }

    }

    //=================================================================================================
    // Now Assign Face Vector
    for (int idx=0 ; idx<faceOrder.size() ; idx++) {

        Mesh._faces.push_back( faceMap[faceOrder[idx]] );

        // Add face to elements
        for (std::shared_ptr<element> e : Mesh._faces[idx]->get_elements()) {
            Mesh._elements[e->get_id()]->add_face( Mesh._faces[idx] );
        }
    }

    //=================================================================================================
    // Loop through boundaries and assign faces vector
    for ( int b=0 ; b<Mesh._boundaries.size() ; b++ )
    {
        // First check if BC has faces
        if ( Mesh._boundaries[b]->get_faces().size() != 0 ) {
            continue;
        }

        // Make sure boundary face seeds are defined
        assert( Mesh._boundaries[b]->get_faceSeeds().size() > 0 && "Boundary face seeds are not defined" );

        // Loop through faces in BC and add
        for (int f=0 ; f<Mesh._boundaries[b]->get_faceSeeds().size() ; f++)
        {
            // Add face
            Mesh._boundaries[b]->add_face( faceMap[Mesh._boundaries[b]->get_faceSeeds()[f]] );

            // Set face to BC
            Mesh._faces[ faceMap[Mesh._boundaries[b]->get_faceSeeds()[f]]->get_id() ]->set_boundary(Mesh._boundaries[b]->get_id());
        }
    }

    // Now that faces have been assigned to elements, and boundary faces defined, initialize elements
    for (int e=0 ; e<Mesh._elements.size() ; e++) {
        Mesh._elements[e]->initializeExterior();
    }


    auto end = std::chrono::steady_clock::now();
    auto diff = end-start;
    if (_verbose) std::cout << "    Element Instantiation time: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

}

// * * * * * * * * * * * * * *  Update Nodes * * * * * * * * * * * * * * * //
void MESH::read_base::updateNodes()
{
    auto start = std::chrono::steady_clock::now();

    // Add elements to nodes
    for (int e=0 ; e<Mesh._elements.size() ; e++) {
        for (int n=0 ; n<Mesh._elements[e]->get_nodes().size() ; n++) {
            Mesh._nodes[Mesh._elements[e]->get_nodes()[n]->get_id()]->add_element( Mesh._elements[e] );
        }
    }

    // Add faces to nodes
    for (int f=0 ; f<Mesh._faces.size() ; f++) {
        for (int n=0 ; n<Mesh._faces[f]->get_nodes().size() ; n++) {
            Mesh._nodes[Mesh._faces[f]->get_nodes()[n]->get_id()]->add_face( Mesh._faces[f] );
            if (Mesh._faces[f]->is_boundaryFace()) {
                Mesh._nodes[Mesh._faces[f]->get_nodes()[n]->get_id()]->set_boundary(true);
            }
        }
    }
    
    // Calculate distance weights
    for (int n=0 ; n<Mesh._nodes.size() ; n++) {
        Mesh._nodes[n]->calculateElementDistanceWeights();
    }

    auto end = std::chrono::steady_clock::now();
    if (_verbose) std::cout << "    node connectivity reading time: " << std::chrono::duration<double, std::milli>(end-start).count() << " ms" << std::endl;
}




