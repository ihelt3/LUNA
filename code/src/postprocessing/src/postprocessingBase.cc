/*------------------------------------------------------------------------*\
**  
**  @file:      postprocessingBase.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation of base classes for post-processing framework
**
\*------------------------------------------------------------------------*/

#include "postprocessingBase.hh"

// cross compatability considerations
#ifdef _WIN32
    #include <direct.h> // For Windows
#else
    #include <sys/stat.h> // For POSIX
#endif

/*------------------------------------------------------------------------*\
**  Class postprocessingBase Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //

POSTPROCESSING::postprocessingBase::postprocessingBase(std::shared_ptr<SOLVER::Solver> solver)
:
    _solver(solver)
{
    std::cout << "Initializing post-processing base class..." << std::endl;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    // First, get parameters from the solver
    _mesh = _solver->get_mesh();
    _solver_variables = _solver->get_variables();

    // Get all components from mesh (domain + all boundaries)
    _all_components = {"domain"};
    for (const std::shared_ptr<MESH::Boundary>& boundary : _solver->get_mesh()->get_boundaries()) {
        _all_components.push_back(boundary->get_name());
    }

};



// * * * * * * * * * * * * * * Make Directory * * * * * * * * * * * * * * * //
bool POSTPROCESSING::postprocessingBase::make_dir(std::string dir_name)
{
    // default dir name
    if (dir_name.empty()) {
        dir_name = output_dir;
    }

    // os system handling call to make the directory
    #ifdef _WIN32
        int result = _mkdir(dir_name.c_str()); // windows implementation
    #else
        int result = mkdir(dir_name.c_str(), 0777); // POSIX version
    #endif

    if (result == 0) {
        std::cout << "Output directory " << dir_name << " created." << std::endl;
    }
    else if (result == -1) {
        // std::cout << "Output directory " << dir_name << " already exists." << std::endl;
    }
    else {
        std::cerr << "Failed to create output directory: " << dir_name << std::endl;
        return false;
    }
    
    return true;
};


// * * * * * * * * * * * * * * Check input components * * * * * * * * * * * * * * * //
bool POSTPROCESSING::postprocessingBase::check_components(std::vector<std::string> components)
{
    for (const auto& comp : components)
    {
        if (std::find(_all_components.begin(), _all_components.end(), comp) == _all_components.end())
        {
            std::cerr << "ERROR: component " << comp << " does not exist!" << std::endl;
            return false;
        }
    }
    return true;
}


// * * * * * * * * * * * * * * Check input variables * * * * * * * * * * * * * * * //
bool POSTPROCESSING::postprocessingBase::check_variables(std::vector<std::string> variables)
{
    for (const auto& var : variables)
    {
        if (std::find(_solver_variables.begin(), _solver_variables.end(), var) == _solver_variables.end())
        {
            std::cerr << "ERROR: variable " << var << " does not exist in the solver!" << std::endl;
            return false;
        }
    }
    return true;
}