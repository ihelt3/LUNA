/*------------------------------------------------------------------------*\
**  
**  @file:      writeASCII.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation of functions to write ASCII files
**
\*------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>

#include "writeASCII.hh"
#include "BoundaryConditions.hh"

/*------------------------------------------------------------------------*\
**  Class postprocessingBase Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //

POSTPROCESSING::writeASCII::writeASCII(std::shared_ptr<SOLVER::Solver> solver)
: 
    postprocessingBase(solver) 
{
    std::cout << "Initializing ASCII writer..." << std::endl;
}


// * * * * * * * * * * * * * Write Specified Boundaries and variables * * * * * * * * * * * * * * //

// NOTE: this function is going to be terribly ugly, I just need to get something to visualize my results
void POSTPROCESSING::writeASCII::write(std::vector<std::string> components, std::vector<std::string> variables) 
{
    std::cout << "Writing cell ASCII data..." << std::endl;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    // Input checking/correcting

    // Make sure each component exists
    check_components(components);

    // Make sure all variables exist in solver
    check_variables(variables);

    // Ensure output directory exists
    make_dir(output_dir);

    // Add coordinate data to variables array
    std::vector<std::string> coordinates = {"x","y","z"};
    for (int d=0 ; d<_mesh->get_dimension() ; d++ )
    {
        if (std::find(variables.begin(), variables.end(), coordinates[d]) == variables.end()) 
        {
            variables.insert(variables.begin()+d,coordinates[d]);
        }
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    // Write the header

    // Create and open a text file
    std::string outputName = output_dir + "/" + "output.txt";
    std::ofstream outfile(outputName);
    for ( const auto& v : variables )
    {
        outfile << v << ", ";
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    // Write the data
    for (const auto& comp : components)
    {
        // WRITING THE DOMAIN
        if (comp == "domain") 
        {
            // Loop over cells
            for (int c=0 ; c<_mesh->get_elements().size() ; c++) 
            {
                outfile << "\n";
                // Loop through variables
                for (const auto& v : variables)
                {
                    if      (v == "x")      { outfile << _mesh->get_elements()[c]->get_centroid()[0]; }
                    else if (v == "y")      { outfile << _mesh->get_elements()[c]->get_centroid()[1]; }
                    else if (v == "z")      { outfile << _mesh->get_elements()[c]->get_centroid()[2]; }
                    else if (v == "u")      { outfile << _solver->get_cellVelocityField().get_internal()[c][0]; }
                    else if (v == "v")      { outfile << _solver->get_cellVelocityField().get_internal()[c][1]; }
                    else if (v == "w")      { outfile << _solver->get_cellVelocityField().get_internal()[c][2]; }
                    else if (v == "p")      { outfile << _solver->get_cellPressureField().get_internal()[c]; }

                    outfile << ", ";
                }
                
            }
        }
        // WRITING BOUNDARY CONDITIONS
        else 
        {
            // Get Boundary idx
            int idx = _mesh->get_boundaryIdx(comp);
            int id = _mesh->get_boundaryID(comp);

            std::shared_ptr<BOUNDARIES::BoundaryCondition> bc =  _solver->get_boundaryCondition(id);
            
            // Loop over faces
            for (const std::shared_ptr<MESH::face>& f : _mesh->get_boundaries()[idx]->get_faces() )
            {
                outfile << "\n";
                // Loop over variables
                for (const auto& v : variables) 
                {
                    if      (v == "x") { outfile << f->get_centroid()[0]; }
                    else if (v == "y") { outfile << f->get_centroid()[1]; }
                    else if (v == "z") { outfile << f->get_centroid()[2]; }
                    else if (v == "u") { outfile << bc->get_velocity(f->get_id())[0]; }
                    else if (v == "v") { outfile << bc->get_velocity(f->get_id())[1]; }
                    else if (v == "w") { outfile << bc->get_velocity(f->get_id())[2]; }
                    else if (v == "p") { outfile << bc->get_pressure(f->get_id()); }

                    outfile << ", ";
                }
            }
        }
    }

    // Close the text file
    outfile.close();
}

// * * * * * * * * * * * * * Write Specified Boundaries and variables of face variables * * * * * * * * * * * * * * //
void POSTPROCESSING::writeASCII::write_faces(std::vector<std::string> components, std::vector<std::string> variables)
{
    std::cout << "Writing face ASCII data..." << std::endl;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    // Input checking/correcting

    // Make sure each component exists
    check_components(components);

    // Make sure all variables exist in solver
    check_variables(variables);

    // Ensure output directory exists
    make_dir(output_dir);

    // Add coordinate data to variables array
    std::vector<std::string> coordinates = {"x","y","z"};
    for (int d=0 ; d<_mesh->get_dimension() ; d++ )
    {
        if (std::find(variables.begin(), variables.end(), coordinates[d]) == variables.end()) 
        {
            variables.insert(variables.begin()+d,coordinates[d]);
        }
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    // Write the header

    // Create and open a text file
    std::string outputName = output_dir + "/" + "output_faces.txt";
    std::ofstream outfile(outputName);
    for ( const auto& v : variables )
    {
        outfile << v << ", ";
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    // Write the data
    for (const auto& comp : components)
    {
        // WRITING THE DOMAIN
        if (comp == "domain") 
        {
            // Loop over cells
            for (int f=0 ; f<_mesh->get_faces().size() ; f++) 
            {
                outfile << "\n";
                // Loop through variables
                for (const auto& v : variables)
                {
                    if      (v == "x")      { outfile << _mesh->get_faces()[f]->get_centroid()[0]; }
                    else if (v == "y")      { outfile << _mesh->get_faces()[f]->get_centroid()[1]; }
                    else if (v == "z")      { outfile << _mesh->get_faces()[f]->get_centroid()[2]; }
                    else if (v == "u")      { outfile << _solver->get_faceVelocityField().get_internal()[f][0]; }
                    else if (v == "v")      { outfile << _solver->get_faceVelocityField().get_internal()[f][1]; }
                    else if (v == "w")      { outfile << _solver->get_faceVelocityField().get_internal()[f][2]; }
                    else if (v == "p")      { outfile << _solver->get_facePressureField().get_internal()[f]; }
                    else if (v == "mdotf")  { outfile << _solver->get_faceMassFluxField().get_internal()[f]; }

                    outfile << ", ";
                }
                
            }

            // Exit the loop because domain actually writes everything
            break;
        }
        // WRITING BOUNDARY CONDITIONS
        else 
        {
            // Get Boundary idx
            int idx = _mesh->get_boundaryIdx(comp);
            int id = _mesh->get_boundaryID(comp);

            std::shared_ptr<BOUNDARIES::BoundaryCondition> bc =  _solver->get_boundaryCondition(id);
            
            // Loop over faces
            for (const std::shared_ptr<MESH::face>& f : _mesh->get_boundaries()[idx]->get_faces() )
            {
                outfile << "\n";
                // Loop over variables
                for (const auto& v : variables) 
                {
                    if      (v == "x")      { outfile << f->get_centroid()[0]; }
                    else if (v == "y")      { outfile << f->get_centroid()[1]; }
                    else if (v == "z")      { outfile << f->get_centroid()[2]; }
                    else if (v == "u")      { outfile << bc->get_velocity(f->get_id())[0]; }
                    else if (v == "v")      { outfile << bc->get_velocity(f->get_id())[1]; }
                    else if (v == "w")      { outfile << bc->get_velocity(f->get_id())[2]; }
                    else if (v == "p")      { outfile << bc->get_pressure(f->get_id()); }
                    else if (v == "mdotf")  { outfile << bc->get_massFlux(f->get_id()); }

                    outfile << ", ";
                }
            }
        }
    }

    // Close the text file
    outfile.close();
}
