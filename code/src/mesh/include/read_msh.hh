/*------------------------------------------------------------------------*\
**  
**  @file:      read_msh.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Read gmsh .msh file format
**
\*------------------------------------------------------------------------*/

#ifndef _READ_MSH_HH_
#define _READ_MSH_HH_

#include <map>
#include "read_base.hh"


namespace MESH
{

/*------------------------------------------------------------------------*\
**  Class read_msh Declaration
\*------------------------------------------------------------------------*/

class read_msh
:
    public read_base
{
public:
    
    // Define struct for physical names
    struct physicalNamesStruct {
        int dimension;
        std::string name;
    };

    // Type Definitions
    using physicalNamesMap = typename std::map<int,physicalNamesStruct>;

private:
    // Private Data
    double _version;
    int _filetype, _datasize;
    int _nPhysicalNames;
    physicalNamesMap _physicalNames;
    int _nPoints, _nCurves, _nSurfaces, _nVolumes;

    // Private Member Functions
    void parseMeshFormat();
    void parsePhysicalNames();
    void parseEntities();
    void parseNodes();
    void parseElements();


public:
    // Constructor
    read_msh(std::filesystem::path mshFilePath);

    
};


} // End namespace MESH


#endif // _READ_MSH_HH_
