/*------------------------------------------------------------------------*\
**  
**  @file:      NACA0012.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Example of LUNA2D solver for NACA 0012 airfoil
**
\*------------------------------------------------------------------------*/

#include <filesystem>
#include <memory>

#include "read_su2.hh"
#include "SIMPLE.hh"
#include "BoundaryConditions.hh"
#include "writeASCII.hh"
#include "BoundaryConditions.hh"
#include "wall.hh"
#include "inlet.hh"
#include "outlet.hh"

int main()
{
    // Load Mesh
    std::filesystem::path meshFile("NACA_0012.su2");

    // Read meshes
    MESH::read_su2 testMesh(meshFile);

    // Store meshes
    std::shared_ptr<MESH::mesh> su2Mesh = std::make_shared<MESH::mesh>(testMesh.get_mesh());

    // Construct SIMPLE solver
    // NOTE: this HAS to be a smart pointer so that the boundary condition solver pointers do not go out of scope and return null pointers
    std::shared_ptr<SOLVER::SIMPLE> simpleSolver = std::make_shared<SOLVER::SIMPLE>(su2Mesh);


    // Set boundary conditions
    MATH::Vector velocity(std::vector<double>{1.0,0.0});  // m/s
    BOUNDARIES::inlet inlet(simpleSolver,"inlet",velocity);
    BOUNDARIES::outlet outlet(simpleSolver,"outlet",0.0);
    BOUNDARIES::outlet farfield(simpleSolver,"farfield",0.0);
    BOUNDARIES::viscousWallBC airfoil(simpleSolver,"airfoil");

    simpleSolver->setBoundaryCondition(std::make_shared<BOUNDARIES::inlet>(inlet));
    simpleSolver->setBoundaryCondition(std::make_shared<BOUNDARIES::outlet>(outlet));
    simpleSolver->setBoundaryCondition(std::make_shared<BOUNDARIES::outlet>(farfield));
    simpleSolver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(airfoil));

    // Solve the system
    simpleSolver->iter = 1;
    simpleSolver->solve();
    std::cout << "Solver Complete" << std::endl;

    // Print Output
    POSTPROCESSING::writeASCII ascii(simpleSolver);
    std::vector<std::string> components = {"domain"};
    std::vector<std::string> variables = {"u","v","p"};
    ascii.write(components,variables);

    ascii.write_faces({"domain"},{"u","v","p"});

    return 0;
}