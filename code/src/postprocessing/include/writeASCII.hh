/*------------------------------------------------------------------------*\
**  
**  @file:      writeASCII.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Header file of functions for writing ASCII files
**
\*------------------------------------------------------------------------*/

#ifndef _WRITEASCII_HH_
#define _WRITEASCII_HH_

#include <memory>

#include "postprocessingBase.hh"

namespace POSTPROCESSING {

/*------------------------------------------------------------------------*\
**  writeASCII Class
\*------------------------------------------------------------------------*/
class writeASCII
:
    public postprocessingBase
{
public:
    // Constructors
        // Construct with solver and name
        writeASCII(std::shared_ptr<SOLVER::Solver> solver);

    // Methods
        // write ASCII of domain
        void write(); 
        // write ASCII of specific boundaries/domains and variables
        void write(std::vector<std::string> boundaries, std::vector<std::string> variables);
        // write ASCII of specific face variables
        void write_faces(std::vector<std::string> boundaries, std::vector<std::string> variables);
};


}

#endif // _WRITEASCII_HH_