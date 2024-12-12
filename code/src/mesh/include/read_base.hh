/*------------------------------------------------------------------------*\
**  
**  @file:      read_base.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header file for read mesh base class
**
\*------------------------------------------------------------------------*/

#ifndef _READ_BASE_HH_
#define _READ_BASE_HH_

#include <filesystem>
#include <fstream>
#include <iostream>
#include <cassert>
#include <string>

#include "MeshEntities.hh"
#include "mesh.hh"

namespace MESH
{

/*------------------------------------------------------------------------*\
**  Class read_base Declaration
\*------------------------------------------------------------------------*/

class read_base {

protected:
    // Private Member Data
        // mesh file
        std::ifstream _file; 
        // dimension of mesh file
        int _dimension; 
        // Mesh data
        int _nElements, _nNodes, _nBCs;
        // elements vector
        std::vector<element> _elements;
        std::vector<face> _faces;
        // Nodes Vector
        std::vector<node> _nodes;
        // Boundary Conditions Vector
        std::vector<Boundary> _BCs;  
        // // element Connectivity tensor
        // std::vector<std::vector<int>> _elementConnectivity;
        // // Boundary Condition Connectivity tensor
        // std::vector<std::vector<int>> _BCConnectivity;
    

public:
    // Constructor
    read_base(std::filesystem::path filePath);


    // Destructor
    ~read_base();

    // Data

    // Member Functions
        // Check for string in file
        bool check_in_file(const std::string str);
        // Instantiate elements from nodal data
        void instantiateElements();
        // Instantiate faces from element data (THIS INCLUDES ELEMENT CONNECTIVITY)
        void instantiateFaces();
        // Assign Node elements
        void updateNodes();
        // get line with specific string
        std::string get_line_of_str(const std::string, const std::string);

    // get methods
        // return elements vector
        std::vector<element>& get_elements() { return _elements; };
        // return nodes vector
        const std::vector<node>& get_nodes() const { return _nodes; };
        // // return element connectivity vector
        // const std::vector<std::vector<int>>& get_elementConnectivity() const { return _elementConnectivity; };
        // // return boundary condition connectivity vector
        // const std::vector<std::vector<int>>& get_BCConnectivity() const { return _BCConnectivity; };
        // return mesh object
        mesh get_mesh();
};

}


#endif // _READ_BASE_HH_