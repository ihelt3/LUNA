/*------------------------------------------------------------------------*\
**  
**  @file:      test_writeASCII.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     test methods to write ASCII files
**
\*------------------------------------------------------------------------*/

#include <filesystem>
#include <vector>
#include <memory>

#include "gtest/gtest.h"
#include "writeASCII.hh"
#include "Solver.hh"
#include "mesh.hh"
#include "read_su2.hh"
#include "BoundaryConditions.hh"
#include "wall.hh"

class writeASCII_test : public ::testing::Test
{

protected:
    // Member data
    std::shared_ptr<MESH::mesh> su2Mesh;
    std::shared_ptr<SOLVER::Solver> solver;

    // Constructor
    writeASCII_test()
    {
        complexInitialization();
    }

    // Initializer
    void complexInitialization() {
        // Simple mesh to test with
        std::filesystem::path meshFile("test/Common/testMesh.su2");

        // Read mesh
        MESH::read_su2 testMesh(meshFile);

        // Store mesh
        su2Mesh = std::make_shared<MESH::mesh>(testMesh.get_mesh());

        // Construct solver
        solver = std::make_shared<SOLVER::Solver>(su2Mesh);

        // Set Boundary conditions
        BOUNDARIES::viscousWallBC testWallLower(solver,"lower");
        BOUNDARIES::viscousWallBC testWallUpper(solver,"upper");
        testWallLower.set_velocity(MATH::Vector(std::vector<double>{1.0,2.0}));
        solver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(testWallLower));
        solver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(testWallUpper));
    }

};


TEST_F(writeASCII_test, test_writeASCII) {

    // Arrange
    POSTPROCESSING::writeASCII ascii(solver);
    std::vector<std::string> components = {"domain","lower"};
    std::vector<std::string> variables = {"u","v","p"};

    // Act
    ascii.write(components,variables);

    // Assert

}

TEST_F(writeASCII_test, test_writeASCII_faces) {

    // Arrange
    POSTPROCESSING::writeASCII ascii(solver);
    std::vector<std::string> components = {"domain"};
    std::vector<std::string> variables = {"u","mdotf","p"};

    // Act
    ascii.write_faces(components,variables);

    // Assert
}
