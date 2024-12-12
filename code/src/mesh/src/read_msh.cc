/*------------------------------------------------------------------------*\
**  
**  @file:      read_msh.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Read gmsh .msh file format
**
\*------------------------------------------------------------------------*/

#include "../include/read_msh.hh"

/*------------------------------------------------------------------------*\
**  Class read_msh Implementation
\*------------------------------------------------------------------------*/

// Constructor
MESH::read_msh::read_msh(std::filesystem::path mshFilePath) : read_base(mshFilePath) {
    parseMeshFormat();
    parsePhysicalNames();
}

// Parsing mesh format
void MESH::read_msh::parseMeshFormat() {
    std::string search="$MeshFormat";

    if (check_in_file(search)) {
        _file >> _version >> _filetype >> _datasize;
    }
    else {
        std::cerr << "Could not find " << search << " in .msh file!" << std::endl;
    }
}

// Parse Physical Names
void MESH::read_msh::parsePhysicalNames() {
    std::string search="$PhysicalNames";

    if (check_in_file(search)) {
        // Get number of physical Names
        _file >> _nPhysicalNames;

        // Loop through physical names and create map to physical names struct
        int dim, id;
        std::string name;
        for (int physicalTagInd=0 ; physicalTagInd < _nPhysicalNames ; physicalTagInd++) {
            _file >> dim >> id >> name;
            name.erase(name.begin());
            name.erase(name.end()-1);
            physicalNamesStruct physStruct;
            physStruct.dimension = dim;
            physStruct.name = name;
            _physicalNames.insert(std::pair<int,physicalNamesStruct>(id,physStruct));
        }
    }
    else {
        std::cerr << "Could not find " << search << " in .msh file!" << std::endl;
    }

    // for (physicalNamesMap::iterator it=_physicalNames.begin() ; it != _physicalNames.end() ; it++) {
    //     std::cout << "id: " << it->first << "   dimension: " << it->second.dimension << "   name: " << it->second.name << std::endl;
    // }
}

// Parse Entities
void MESH::read_msh::parseEntities() {
    std::string search="$Entities";

    if (check_in_file(search)) {
        _file >> _nPoints >> _nCurves >> _nSurfaces >> _nVolumes;
        // assert(_nVolumes==0 && )
    }
    else {
        std::cerr << "Could not find " << search << " in .msh file!" << std::endl;
    }
}

// Parse Nodes


// Parse Elements


