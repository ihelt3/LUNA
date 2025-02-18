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

class read_base 
{
public:
    // Constructor
    read_base(std::filesystem::path filePath, bool verbose=true );

    // Destructor
    ~read_base();

    // Data

    // Member Functions
        // Check for string in file
        bool check_in_file(const std::string str);
        // Instantiate elements from nodal data
        void instantiateElements();
        // Assign Node elements
        void updateNodes();
        // get line with specific string
        std::string get_line_of_str(const std::string, const std::string);

    // get methods
        // return elements vector
        std::vector<std::shared_ptr<element>>& get_elements() { return Mesh._elements; };
        // return nodes vector
        const std::vector<std::shared_ptr<node>>& get_nodes() const { return Mesh._nodes; };
        // return mesh object
        mesh get_mesh();

protected:
    // Private Member Data
        // mesh file
        std::ifstream _file; 
        // Mesh data
        int _nElements, _nNodes, _nBCs;
        // elements map
        mesh Mesh;
        // option for verbose output
        bool _verbose;
    
};

}


#endif // _READ_BASE_HH_