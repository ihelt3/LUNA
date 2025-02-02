/*------------------------------------------------------------------------*\
**  
**  @file:      read_su2.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Read .su2 file format
**
\*------------------------------------------------------------------------*/

#include <chrono>
#include "read_su2.hh"

/*------------------------------------------------------------------------*\
**  Class read_msh Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
MESH::read_su2::read_su2(std::filesystem::path su2FilePath) : read_base(su2FilePath) {
    std::cout << "Reading su2 mesh..." << std::endl;
    auto start = std::chrono::steady_clock::now();

    std::string line;

    // NEED TO UPDATE THIS TO READ NODES FIRST

    // Loop through file, send to methods to parse file as needed
    while (!_file.eof()) {

        getline(_file,line);
        line = line.substr(0,line.find(_comment));

        // Check for NDIME
        if (line.find("NDIME") != std::string::npos) {
            std::cout << "  Getting dimensional information..." << std::endl;
            line = line.erase(0,line.find(_delimiter)+1);
            Mesh._dimension = std::stoi(line.c_str());
        }
        // Check for NELEM
        else if (line.find("NELEM") != std::string::npos) {
            std::cout << "  Reading element data..." << std::endl;
            line = line.erase(0,line.find(_delimiter)+1);
            _nElements = std::stoi(line.c_str());
            parseElements();
        }
        else if (line.find("NPOIN") != std::string::npos) {
            std::cout << "  Reading nodal data..." << std::endl;
            line = line.erase(0,line.find(_delimiter)+1);
            _nNodes = std::stoi(line.c_str());
            parseNodes();
        }
        else if (line.find("NMARK") != std::string::npos) {
            std::cout << "  Reading boundary conditions..." << std::endl;
            line = line.erase(0,line.find(_delimiter)+1);
            _nBCs = std::stoi(line.c_str());
            parseBCs();
        }
    }

    std::cout << "  instantiating elements from node list..." << std::endl;
    instantiateElements();
    std::cout << "  Updating Nodes..." << std::endl;
    updateNodes();

    // Time mesh reading
    auto end = std::chrono::steady_clock::now();
    auto diff = end-start;
    std::cout << "Mesh read in " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    // done!
    std::cout << "su2 mesh read!" << std::endl;
}


// * * * * * * * * * * * * * *  elementFromLine * * * * * * * * * * * * * * * //
// NOTE: This initializes only the node ID vector, element still needs to be initialized
MESH::element MESH::read_su2::elementFromLine(std::string line, bool subElement, int id)
{
    int nodei;
    int elementType;

    std::istringstream iss(line);
    iss >> elementType;
    su2ElementType su2element = static_cast<su2ElementType>(elementType);

    // Get local variable naming scheme
    elementTypeEnum elementTypei;
    if (su2element == su2ElementType::LINE) elementTypei = elementTypeEnum::LINE;
    else if (su2element == su2ElementType::TRIANGLE) elementTypei = elementTypeEnum::TRIANGLE;
    else if (su2element == su2ElementType::QUADRILATERAL) elementTypei = elementTypeEnum::QUADRILATERAL;
    else if (su2element == su2ElementType::TETRAHEDRAL) elementTypei = elementTypeEnum::TETRAHEDRAL;
    else if (su2element == su2ElementType::HEXAHEDRAL) elementTypei = elementTypeEnum::HEXAHEDRAL;
    else if (su2element == su2ElementType::PRISM) elementTypei = elementTypeEnum::PRISM;
    else if (su2element == su2ElementType::PYRAMID) elementTypei = elementTypeEnum::PYRAMID;

    std::vector<int> nodeID_vec;
    for (int nodes=0 ; nodes < int(_elementType2numberNodes[su2element]) ; nodes++) {
        iss >> nodei;
        nodeID_vec.push_back(nodei);
    }
    
    element elementOut(id,elementTypei,nodeID_vec,subElement);
    elementOut.hash();

    return elementOut;
}


// * * * * * * * * * * * * * *  parseElements * * * * * * * * * * * * * * * //
void MESH::read_su2::parseElements() {

    _elementType2numberNodes.emplace(su2ElementType::LINE           ,numberOfNodes::LINE);
    _elementType2numberNodes.emplace(su2ElementType::TRIANGLE       ,numberOfNodes::TRIANGLE);
    _elementType2numberNodes.emplace(su2ElementType::QUADRILATERAL  ,numberOfNodes::QUADRILATERAL);
    _elementType2numberNodes.emplace(su2ElementType::TETRAHEDRAL    ,numberOfNodes::TETRAHEDRAL);
    _elementType2numberNodes.emplace(su2ElementType::HEXAHEDRAL     ,numberOfNodes::HEXAHEDRAL);
    _elementType2numberNodes.emplace(su2ElementType::PRISM          ,numberOfNodes::PRISM);
    _elementType2numberNodes.emplace(su2ElementType::PYRAMID        ,numberOfNodes::PYRAMID);

    std::string line;   
    
    int temp_str;
    int id = 0;
    
    // Get element type, then read appropriate number of lines
    // NOTE: some legacy su2 files include element ID at the end of each line, which is implied by the order of elements, so we ignore it if it is there
    for (int id=0 ; id < _nElements ; id++) {
        
        // get element from line in file
        getline(_file,line);
        element elementi = elementFromLine(line,false,id);

        // Add element to elements vector
        Mesh._elements.push_back(std::make_shared<element>(elementi));
    }
}


// * * * * * * * * * * * * * *  parseNodes * * * * * * * * * * * * * * * //
void MESH::read_su2::parseNodes() {
    // In
    std::string line;
    double coord;

    for (int node_it=0 ; node_it < _nNodes ; node_it++) {
        getline(_file,line);
        std::istringstream iss(line);
        std::vector<double> coordinates;
        coordinates.reserve(Mesh._dimension);

        for (int dim=0 ; dim<Mesh._dimension ; dim++) {
            iss >> coord;
            coordinates.push_back(coord);
        }
        // instantiate node object
        node nodei(node_it,coordinates);
        Mesh._nodes.push_back( std::make_shared<node>(nodei) );
    }
}

// * * * * * * * * * * * * * *  parseBCs * * * * * * * * * * * * * * * //
void MESH::read_su2::parseBCs() {
    std::string line;
    int nFaces;

    for (int mark=0 ; mark<_nBCs ; mark++) {
        // Get the name and remove any whitespace
        getline(_file,line);
        line.erase(0,line.find(_delimiter)+1);
        line.erase(std::remove_if(line.begin(),line.end(),::isspace),line.end());
        std::string name;
        name = line;

        // Determine the number of elements on the boundary
        getline(_file,line);
        line = line.erase(0,line.find(_delimiter)+1);
        
        // Get number of elements in this boundary
        nFaces = std::stoi(line);

        // Initialize list of boundary elements
        std::vector<std::string> BC_faceSeeds;
        BC_faceSeeds.reserve(nFaces);

        // Loop through elements and grab number of nodes per
        for (int facei=0 ; facei<nFaces ; facei++)
        {
            // Get Element type and convert to local naming convention
            getline(_file,line);

            // BC elements should be set as sub-elements
            element thisElement = elementFromLine(line,true,facei);
            // Instantiate face as boundary face (true)
            face thisFace(thisElement,true);
            
            // Add to element list
            BC_faceSeeds.push_back(thisFace.get_seed());
        }

        // Define boundary object
        Boundary thisBC(name,BC_faceSeeds,-mark-1);

        // Add boundary condition to list
        Mesh._boundaries.push_back( std::make_shared<Boundary>(thisBC) );
    }
}

