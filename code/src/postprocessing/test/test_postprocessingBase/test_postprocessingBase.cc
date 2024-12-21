/*------------------------------------------------------------------------*\
**  
**  @file:      test_postprocessingBase.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     test methods to postprocessing base class
**
\*------------------------------------------------------------------------*/

#include <filesystem>
#include <vector>
#include <memory>

#include "gtest/gtest.h"
#include "postprocessingBase.hh"
#include "Solver.hh"
#include "mesh.hh"
#include "read_su2.hh"
#include "BoundaryConditions.hh"
#include "wall.hh"

class postprocessing_test : public ::testing::Test
{
public:
    // Constructor
    postprocessing_test() {
        complexInitialization();
    }

protected:
    // Member data
    std::shared_ptr<MESH::mesh> su2Mesh;
    std::shared_ptr<SOLVER::Solver> solver;

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


TEST_F(postprocessing_test, test_mkdir) {

    // Arrange
    POSTPROCESSING::postprocessingBase post(solver);

    // Act
    bool result = post.make_dir(post.output_dir);

    std::cout << "Output directory created!" << std::endl;
    ASSERT_EQ(result, true);
    EXPECT_TRUE(std::filesystem::exists(post.output_dir));

}