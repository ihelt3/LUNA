/*------------------------------------------------------------------------*\
**  
**  @file:      test_su2.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit tests for reading su2 file
**
\*------------------------------------------------------------------------*/

#include <filesystem>
#include <vector>
#include <memory>

#include "gtest/gtest.h"

#include "SIMPLE.hh"
#include "read_su2.hh"
#include "mesh.hh"
#include "wall.hh"

/*------------------------------------------------------------------------*\
**  Test Fixture
\*------------------------------------------------------------------------*/

// Inherit gtest's ::testing::Test class, making it a fixture
class simple_test : public ::testing::Test 
{

protected:
    std::shared_ptr<MESH::mesh> su2Mesh;
    // SOLVER::SIMPLE simpleSolver;
    std::shared_ptr<SOLVER::SIMPLE> simpleSolver;

    // Initializer
    void complexInitialization() {
        // Simple mesh to test with
        std::filesystem::path meshFile(COMMON_DIR "/su2/testMesh.su2");

        // Read meshes
        MESH::read_su2 testMesh(meshFile);

        // Store meshes
        su2Mesh = std::make_shared<MESH::mesh>(testMesh.get_mesh());

        // Construct SIMPLE solver
        simpleSolver = std::make_shared<SOLVER::SIMPLE>(su2Mesh);

        BOUNDARIES::viscousWallBC testWallLower(simpleSolver,"lower");
        BOUNDARIES::viscousWallBC testWallUpper(simpleSolver,"upper");
        simpleSolver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(testWallLower));
        simpleSolver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(testWallUpper));
    }

    // Node Coordinates
    std::vector<std::vector<double>> nodeCoordinates = {
        {1.0,0.0} , {2.0,0.0} , {0.0,1.0} , {1.0,1.0} , {2.0,1.0} , {3.0,1.0} , 
        {0.0,2.0} , {1.0,2.0} , {2.0,2.0} , {3.0,2.0} , {1.0,3.0} , {2.0,3.0}
    };


public:

    // Constructor
    simple_test() 
    {
        complexInitialization();
    };

    

};


// * * * * * * * * * * * * * *  test cell face velocities * * * * * * * * * * * * * * * //
TEST_F(simple_test, testFaceVelocities)
{
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    // Trivial Case
    
    // Arrange 

    // Act
    simpleSolver->computeFaceVelocities();
    const auto& faceVelocityField = simpleSolver->get_faceVelocityField();

    std::vector<MATH::Vector> faceVelocities = faceVelocityField.get_internal();
    
    // Assert
    ASSERT_EQ(faceVelocities.size(), simpleSolver->get_mesh()->get_faces().size());
    for (int face=0 ; face<simpleSolver->get_mesh()->get_faces().size() ; face++) {
        ASSERT_DOUBLE_EQ(faceVelocities[face][0], 0.0);
        ASSERT_DOUBLE_EQ(faceVelocities[face][1], 0.0);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    // Non-Trivial Case
    
}


// * * * * * * * * * * * * * *  test Solving Momentum System * * * * * * * * * * * * * * * //
// TEST_F(simple_test, testSolvingMomentumSystem)
// {
//     // Arrange
//     // solver->updateMomentumMatrix();
// }

