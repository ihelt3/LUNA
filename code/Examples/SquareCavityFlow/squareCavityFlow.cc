/*------------------------------------------------------------------------*\
**  
**  @file:      squareCavityFlow.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Example of LUNA2D solver for square cavity flow
**
\*------------------------------------------------------------------------*/

#include <filesystem>
#include <memory>

#include "read_su2.hh"
#include "SIMPLE.hh"
#include "BoundaryConditions.hh"
#include "writeASCII.hh"

int main()
{
    // Load Mesh
    std::filesystem::path meshFile("squareCavity_unstruct.su2");

    // Read meshes
    MESH::read_su2 testMesh(meshFile);

    // Store meshes
    std::shared_ptr<MESH::mesh> su2Mesh = std::make_shared<MESH::mesh>(testMesh.get_mesh());

    // Construct SIMPLE solver
    // NOTE: this HAS to be a smart pointer so that the boundary condition solver pointers do not go out of scope and return null pointers
    std::shared_ptr<SOLVER::SIMPLE> simpleSolver = std::make_shared<SOLVER::SIMPLE>(su2Mesh);


    // Set boundary conditions
    UTILITIES::viscousWallBC bottom(simpleSolver,"bottom");
    UTILITIES::viscousWallBC top(simpleSolver,"top");
    top.set_velocity(MATH::Vector(std::vector<double>{0.1,0.0}));
    simpleSolver->setBoundaryCondition(std::make_shared<UTILITIES::viscousWallBC>(bottom));
    simpleSolver->setBoundaryCondition(std::make_shared<UTILITIES::viscousWallBC>(top));

    // Solve the system
    simpleSolver->iter = 3;
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