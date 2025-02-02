/*------------------------------------------------------------------------*\
**  
**  @file:      MeshAdaption.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation of mesh adaption class
**
\*------------------------------------------------------------------------*/

#include <unordered_map>

#include "MeshAdaption.hh"


/*------------------------------------------------------------------------*\
**  Class meshAdaption Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
MESH::meshAdaption::meshAdaption(std::shared_ptr<mesh> meshPtr)
:
    _meshPtr(meshPtr)
{
    // Only implemented for 2D
    if (_meshPtr->_dimension != 2) {
        std::cerr << "ERROR: MeshAdaption only implemented for 2D meshes." << std::endl;
        exit(1);
    }
}


// * * * * * * * * * * * * * *  adaptCell * * * * * * * * * * * * * * * * //
void MESH::meshAdaption::refineCell(int cellID)
{
    /*
    // Get element
    element e = _meshPtr->_elements[cellID];
    
    // Create a node at the cell centroid
    int nodeID = _meshPtr->_nodes.size();
    node new_node(nodeID, e.get_centroid());

    // Add node to mesh
    _meshPtr->_nodes.push_back(new_node);

    // Create elements from new node
    std::unordered_map<std::string, MESH::face> faceMap;
    MESH::face f;
    int elementID;
    for ( int i=0 ; i<e.get_faces().size() ; i++ )
    {
        f = e.get_faces()[i];

        // define element IDs: use original element ID for first element
        if ( i == 0 ) {
            elementID = cellID;
        }
        else {
            elementID = _meshPtr->_elements.size();
        }

        // Create new element
        element new_e(elementID, elementTypeEnum::TRIANGLE, std::vector<node>{f.get_nodes()[0],f.get_nodes()[1],new_node});

        // Add new faces to facemap
        for ( const auto& face : new_e.get_faces() ) {
            if ((f.get_seed() != face.get_seed()))
            {
                if ( faceMap.find(f.get_seed()) == faceMap.end() ) {
                    // First instance of face
                    faceMap[f.get_seed()] = face;
                }
                else {
                    // Second instance of face, add to all connectivity info
                }
            }
            
        }
        
        // Modify existing face to reflect new connectivity


        // Add new element to mesh


    }

    */
}