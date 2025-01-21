/*------------------------------------------------------------------------*\
**  
**  @file:      test_wall.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit test for wall boundary condition
**
\*------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "Solver.hh"
#include "BoundaryConditions.hh"
#include "mesh.hh"
#include "read_su2.hh"
#include "Vector.hh"
#include "wall.hh"


class test_BCs : public ::testing::Test 
{
protected:
    // Member data
    std::shared_ptr<MESH::mesh> su2Mesh;
    std::shared_ptr<SOLVER::Solver> solver;

    // Initializer
    void complexInitialization() {
        // Simple mesh to test with
        std::filesystem::path meshFile( MESH_DIR "/rectangle.su2"); // NOTE: MESH_DIR is a compile definition defined in CMakeLists.txt

        // Read meshes
        MESH::read_su2 testMesh(meshFile);

        // Store meshes
        su2Mesh = std::make_shared<MESH::mesh>(testMesh.get_mesh());

        // Construct smart pointer to solver
        solver = std::make_shared<SOLVER::Solver>(su2Mesh);
    }


public:
    // Constructor
    test_BCs() {
        complexInitialization();
    }

};


// * * * * * * * * * * * * test test setting velocity * * * * * * * * * * * * * * //
TEST_F(test_BCs, testWallVelocity) {
    BOUNDARIES::viscousWallBC testWall(solver,"lower");

    std::cout << "Testing Wall Velocity" << std::endl;
    MATH::Vector vel(std::vector<double>{1.0,0.0});
    testWall.set_velocity(vel);

    std::cout << "Asserting velocity..." << std::endl;
    ASSERT_DOUBLE_EQ(testWall.get_velocity(2)[0], 1.0);
    ASSERT_DOUBLE_EQ(testWall.get_velocity(2)[1], 0.0);

    std::cout << "Asserting pressure..." << std::endl;
    ASSERT_DOUBLE_EQ(testWall.get_pressure(2), 0.0);
}