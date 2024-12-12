/*------------------------------------------------------------------------*\
**  
**  @file:      read_su2.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Read .su2 file format
**
\*------------------------------------------------------------------------*/

#ifndef _READ_SU2_HH_
#define _READ_SU2_HH_

#include <unordered_map>
#include <sstream>

#include "read_base.hh"

namespace MESH
{

/*------------------------------------------------------------------------*\
**  Class read_msh Declaration
\*------------------------------------------------------------------------*/

class read_su2
:
    public read_base
{
public:
    // Constructor 
    read_su2(std::filesystem::path file);

protected:
    // Protected Data
    enum class su2ElementType {LINE=3, TRIANGLE=5, QUADRILATERAL=9, TETRAHEDRAL=10, HEXAHEDRAL=12, PRISM=13, PYRAMID=14};
    std::unordered_map<su2ElementType, numberOfNodes> _elementType2numberNodes;

    // Member functions
    void parseElements();
    void parseNodes();
    void parseBCs();
    element elementFromLine(std::string, bool, int);
    

private:
    // Private Data
    std::string _comment = "%";
    std::string _delimiter = "=";
};

} // End namespace MESH

#endif // _READ_SU2_HH_