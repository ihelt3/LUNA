/*------------------------------------------------------------------------*\
**  
**  @file:      postprocessingBase.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Header file of base classes for post-processing framework
**
\*------------------------------------------------------------------------*/

#ifndef _postprocessingBase_HH_
#define _postprocessingBase_HH_

#include <memory>
#include <iostream>
#include <string>

#include "Solver.hh"
#include "BoundaryConditions.hh"

namespace POSTPROCESSING {

/*------------------------------------------------------------------------*\
**  postprocessingBase Class
\*------------------------------------------------------------------------*/
class postprocessingBase {

public:

    // Constructors
        postprocessingBase(std::shared_ptr<SOLVER::Solver> solver);

    // Public member data
        // Output folder name
        std::string output_dir = "output";

    // Member functions
        // Make a directory
        bool make_dir(std::string dir_name);

protected:

    // Member data
        // solver pointer
        std::shared_ptr<SOLVER::Solver> _solver;
        // Data initialized from the solver
        std::shared_ptr<MESH::mesh> _mesh;
        std::vector<std::string> _solver_variables;
        std::vector<std::string> _all_components;

    // Member functions
        // Check if input components are valid
        bool check_components(std::vector<std::string> components);
        // Check if input variables are valid
        bool check_variables(std::vector<std::string> variables);


        

};


}

#endif // _postprocessingBase_HH_