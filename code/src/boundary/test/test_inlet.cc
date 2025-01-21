/*------------------------------------------------------------------------*\
**  
**  @file:      test_inlet.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit test for inlet boundary condition
**
\*------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "Solver.hh"
#include "BoundaryConditions.hh"
#include "mesh.hh"
#include "read_su2.hh"
#include "Vector.hh"
#include "inlet.hh"


class test_inlet : public ::testing::Test 
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
    test_inlet() {
        complexInitialization();
    }

};


// * * * * * * * * * * * * test test setting velocity * * * * * * * * * * * * * * //
TEST_F(test_inlet, testInletVelocity) {
    MATH::Vector velocity(std::vector<double>{1.0,0.0});
    BOUNDARIES::inlet testInlet(solver,"lower",velocity);

    std::cout << "Testing Inlet Velocity" << std::endl;

    std::cout << "Asserting velocity..." << std::endl;
    ASSERT_DOUBLE_EQ(testInlet.get_velocity(2)[0], 1.0);
    ASSERT_DOUBLE_EQ(testInlet.get_velocity(2)[1], 0.0);
}
