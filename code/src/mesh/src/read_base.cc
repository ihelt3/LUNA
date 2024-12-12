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
MESH::read_base::read_base(std::filesystem::path filePath) {
    // Open the file
    std::cout << "filepath: " << std::filesystem::absolute(filePath) << " " << filePath << std::endl;
    _file.open(filePath);
    if (!_file.is_open())
    {
        std::cerr << "Cannot open mesh file for reading!";
        exit(1);
    }

}

// * * * * * * * * * * * * * *  Destructor * * * * * * * * * * * * * * * //
MESH::read_base::~read_base() {
    _file.close();
}


// * * * * * * * * * * * * * *  get_mesh * * * * * * * * * * * * * * * //
MESH::mesh MESH::read_base::get_mesh() {
    // Assert variables are initialized
    assert(_dimension > 1);
    assert(!_elements.empty());
    assert(!_nodes.empty());
    assert(!_BCs.empty());
    // assert(!_elementConnectivity.empty());
    // assert(!_BCConnectivity.empty());

    // Return mesh
    return mesh(_dimension, _elements, _faces, _nodes, _BCs);
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
// Instantiate any elements that were constructed with only node ids
void MESH::read_base::instantiateElements()
{
    // Loop through elements 
    std::cout << "   instantiating elements..." << std::endl;
    for (int c=0 ; c<_elements.size() ; c++)
    {
        if (_elements[c].get_nodes().size() == 0) {

            // Nodes have not been assigned: get nodes from IDs and initialize element
            std::vector<int> ids = _elements[c].get_nodeIDs();
            std::vector<node> nodeList;
            for (int n=0 ; n<ids.size() ; n++) {
                nodeList.push_back(_nodes[ids[n]]);
            }

            // Reinitializing element with node vector
            _elements[c].set_nodes(nodeList);
            _elements[c].initialize();
        }
    }

    // Loop through boundary conditions
    std::cout << "   instantiating BCs..." << std::endl;
    for (int b=0 ; b<_BCs.size() ; b++)
    {
        for (int e=0 ; e<_BCs[b].get_faces().size() ; e++)
        {
            if (_BCs[b].get_faces()[e].get_nodes().size() == 0) {
                // Nodes have not been assigned: get nodes from IDs and initialize BC
                std::vector<int> ids = _BCs[b].get_faces()[e].get_nodeIDs();
                std::vector<node> nodeList;
                for (int n=0 ; n<ids.size() ; n++) {
                    nodeList.push_back(_nodes[ids[n]]);
                }
                _BCs[b].get_faces()[e].set_nodes(nodeList);
                _BCs[b].get_faces()[e].initialize();
            }
        }
    }

}

// * * * * * * * * * * * * * *  instantiate Faces from elements * * * * * * * * * * * * * * * //
void MESH::read_base::instantiateFaces()
{
    // Initialize some variables
    std::string faceMapSeed;
    std::unordered_map<int, std::string> faceOrder;
    std::vector<face> efaces;
    std::vector<face> nbfaces;
    int id = 0;
    int nbFaceID;
    int nbFaceIdx;

    // error message
    std::ostringstream errorMsg;


    // make map of hashes
    std::unordered_map<std::string, MESH::face> faceMap;
    auto start = std::chrono::steady_clock::now();
    // Loop through elements
    for (int e=0 ; e<_elements.size() ; e++) {
        for (int f=0 ; f<_elements[e].get_faces().size() ; f++) {

            faceMapSeed = _elements[e].get_faces()[f].get_seed();
            // First instance of face
            if ( faceMap.find(faceMapSeed) == faceMap.end() ) {
                // Assign face order
                faceOrder[id] = faceMapSeed;
                // Assign ID to face
                _elements[e].set_face_id(f,id);
                // Add face to faceMap (and add element to face)
                faceMap[faceMapSeed] = _elements[e].get_faces()[f];
                faceMap[faceMapSeed].set_elements({_elements[e].get_id()});

                // Increment id for new element
                id++;
            }
            // Second instance of face
            else
            {
                // set element face id, and add boundary element
                _elements[e].set_face_id(f,faceMap[faceMapSeed].get_id());
                nbFaceID = faceMap[faceMapSeed].get_elements()[0];
                _elements[e].set_face_connection(f,nbFaceID);
                // add element to face
                faceMap[faceMapSeed].add_element(_elements[e].get_id());
                
                // Add this element to neighbor element's face
                nbfaces = _elements[nbFaceID].get_faces();
                nbFaceIdx = _elements[nbFaceID] == _elements[e].get_faces()[f];
                if (nbFaceIdx == -1) {
                    std::cerr << "ERROR: Hash Collision between elements " << e << " and " << nbFaceID << std::endl;
                }
                _elements[nbFaceID].set_face_connection(nbFaceIdx,_elements[e].get_id());
                
                // Calculate weighted distance between elements
                _elements[e].calculateFaceDistanceWeight(_elements[nbFaceID]);
            }
        }
    }

    auto end1 = std::chrono::steady_clock::now();
    auto diff1 = end1-start;
    std::cout << "    internal face connectivity reading time: " << std::chrono::duration<double, std::milli>(diff1).count() << " ms" << std::endl;

    // Loop though boundary conditions to assign faces
    for (int b=0 ; b<_BCs.size() ; b++) {
        for (int f=0 ; f<_BCs[b].get_faces().size() ; f++) {
            faceMapSeed = _BCs[b].get_faces()[f].get_seed();

            // Make sure an element has already been assigned to this face
            if (!(faceMap[faceMapSeed].get_elements().size() > 0)) {
                errorMsg << "ERROR: Boundary " << _BCs[b].get_name() << " is missing a connecting element on face " << f
                    << std::endl << "    - check if face " << f << " is correctly assigned"
                    << std::endl << "    - ensure there exists a cell that contains face " << f << "(i.e. contains the same nodes)" << std::endl;
                std::cerr << errorMsg.str() << std::endl;
                assert(false);
            }   
            
            // Assign BC to face
            faceMap[faceMapSeed].add_element(_BCs[b].get_id());

            // Set face as a boundary face
            faceMap[faceMapSeed].set_boundaryFace(true);

            // Add boundary to element face
            id = faceMap[faceMapSeed].get_id(); // id of this face

            efaces = _elements[faceMap[faceMapSeed].get_elements()[0]].get_faces(); // faces of connected element
            for (int ef=0 ; ef<efaces.size() ; ef++ ) {
                if (efaces[ef].get_id() == id) {
                    // add BC id to element
                    _elements[faceMap[faceMapSeed].get_elements()[0]].set_face_connection(ef,_BCs[b].get_id());
                    _elements[faceMap[faceMapSeed].get_elements()[0]].set_boundaryFace(ef);
                }
            }

            // Map global face index to local boundary face index (and vice versa)
            _BCs[b].map_global_and_local(faceMap[faceMapSeed].get_id(),f);
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = end-start;
    std::cout << "    boundary face connectivity reading time: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    // Assign faces to vector
    for (int idx=0 ; idx<faceOrder.size() ; idx++) {
        _faces.push_back(faceMap[faceOrder[idx]]);
    }
    
}


// * * * * * * * * * * * * * *  Update Nodes * * * * * * * * * * * * * * * //
void MESH::read_base::updateNodes()
{
    auto start = std::chrono::steady_clock::now();

    // Add elements to nodes
    for (int e=0 ; e<_elements.size() ; e++) {
        for (int n=0 ; n<_elements[e].get_nodes().size() ; n++) {
            _nodes[_elements[e].get_nodes()[n].get_id()].add_element(_elements[e].get_id());
        }
    }

    // Add faces to nodes
    for (int f=0 ; f<_faces.size() ; f++) {
        for (int n=0 ; n<_faces[f].get_nodes().size() ; n++) {
            _nodes[_faces[f].get_nodes()[n].get_id()].add_face(_faces[f].get_id());
            if (_faces[f].is_boundaryFace()) {
                _nodes[_faces[f].get_nodes()[n].get_id()].set_boundary(true);
            }
        }
    }
    
    // Calculate distance weights
    for (int n=0 ; n<_nodes.size() ; n++) {
        _nodes[n].calculateElementDistanceWeights(&_elements);
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "    node connectivity reading time: " << std::chrono::duration<double, std::milli>(end-start).count() << " ms" << std::endl;
}

