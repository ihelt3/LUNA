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

#include "Solver.hh"
#include "read_su2.hh"
#include "mesh.hh"
#include "BoundaryConditions.hh"
#include "wall.hh"

/*------------------------------------------------------------------------*\
**  Test Fixture
\*------------------------------------------------------------------------*/

// Inherit gtest's ::testing::Test class, making it a fixture
class solver_test : public ::testing::Test 
{

protected:
    std::shared_ptr<MESH::mesh> su2Mesh;
    std::shared_ptr<SOLVER::Solver> solver;

    // Initializer
    void complexInitialization() {
        // Simple mesh to test with
        std::filesystem::path meshFile(COMMON_DIR "/su2/testMesh.su2");

        // Read meshes
        MESH::read_su2 testMesh(meshFile);

        // Store meshes
        su2Mesh = std::make_shared<MESH::mesh>(testMesh.get_mesh());

        // Construct solver
        solver = std::make_shared<SOLVER::Solver>(su2Mesh);
    }

    // Node Coordinates
    std::vector<std::vector<double>> nodeCoordinates = {
        {1.0,0.0} , {2.0,0.0} , {0.0,1.0} , {1.0,1.0} , {2.0,1.0} , {3.0,1.0} , 
        {0.0,2.0} , {1.0,2.0} , {2.0,2.0} , {3.0,2.0} , {1.0,3.0} , {2.0,3.0}
    };

    double s2 = sqrt(2.0);
    // Face normal deltas
    std::vector<double> testFaceNormalDeltas = {
        //0    1      2      3    4         5        6      7        8        9       10        11     12   13   14   15        16        17     18
        0.5, 0.5, 5.0/6.0, 0.5, 1.0/3.0, s2/3.0, 1.0/3.0, 2.0/3.0, 1.0/3.0, s2/3.0, 5.0/6.0, 2.0/3.0, 0.5, 0.5, 0.5, s2/3.0, 1.0/3.0, 1.0/3.0, 1.0/3.0
    };

    // perturbed face pressure gradients
    std::vector<std::vector<double>> testFacePressureGradients = {
        {0.0,-0.0} , {0.0,0.0} , {0.0,6.0/5.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0} , {1.5,0.0} , {0.0,0.0} , {-1.5,-1.5} , 
        {0.0,0.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0}
    };

    // perturbed cell pressure gradients
    std::vector<std::vector<double>> testCellPressureGradients = {
        {0.0,1.0} , {0.0,0.0} , {0.0,0.0} , {2.0,0.0} , {-2.0,-2.0} , {0.0,0.0} , {0.0,0.0} , {0.0,0.0}
    };

    // perturbed cell face pressure
    double w1 = (1.0/sqrt(5.0/36.0))/(1.0/sqrt(5.0/36.0) + 2.0);                  //7    8     9   10    11
    std::vector<double> testFacePressure = { 0.0, 0.0, w1*2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.5, w1*1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };


public:

    // Constructor
    solver_test() 
    {
        complexInitialization();
    };

    

};



// * * * * * * * * * * * * * *  test Coordinates * * * * * * * * * * * * * * * //
TEST_F(solver_test, readNodeCoordinates)
{
    // Arrange
    const auto& mesh = solver->get_mesh();

    // Act
    auto nodes = mesh->get_nodes();
    
    // Assert
    ASSERT_EQ(nodes.size(), 12);
    for (int node=0 ; node<nodes.size() ; node++){
        ASSERT_DOUBLE_EQ(nodes[node].get_coordinates()[0], nodeCoordinates[node][0]);
        ASSERT_DOUBLE_EQ(nodes[node].get_coordinates()[1], nodeCoordinates[node][1]);
    }
}


// * * * * * * * * * * * * * *  test cell face normal distance * * * * * * * * * * * * * * * //
TEST_F(solver_test, readFaceNormalDistance)
{
    // Arrange 
    const auto& faceNormalDeltas = solver->get_faceNormalDeltas();

    // Act
    
    // Assert
    ASSERT_EQ(faceNormalDeltas.size(),testFaceNormalDeltas.size());
    for (int face=0 ; face<solver->get_mesh()->get_faces().size() ; face++){
        ASSERT_DOUBLE_EQ(faceNormalDeltas[face], testFaceNormalDeltas[face]);
    }
}

// * * * * * * * * * * * * * *  test setting boundary conditions * * * * * * * * * * * * * * * //
TEST_F(solver_test, test_set_BC)
{
    // - - - - - - - - - - - - - - - - Incomplete Boundaries - - - - - - - - - - - - - - - - //
    // Arrange
    BOUNDARIES::viscousWallBC bc1(solver,"lower");

    // Act
    solver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(bc1));
    
    // Assert
    EXPECT_FALSE(solver->checkBoundaryConditions());

    // - - - - - - - - - - - - - - - - Complete Boundaries - - - - - - - - - - - - - - - - //
    // Arrange
    BOUNDARIES::viscousWallBC bc2(solver,"upper");
    bc2.set_velocity(MATH::Vector(std::vector<double>{2.0,0.0}));

    // Act
    solver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(bc2));

    // Assert
    EXPECT_TRUE(solver->checkBoundaryConditions());
}


// * * * * * * * * * * * * * *  test face pressure gradients * * * * * * * * * * * * * * * //
TEST_F(solver_test, testFacePressureGradients)
{
    // Arrange

    // Act
    const std::vector<MATH::Vector> pressureGradientField = 
        solver->computeFacePressureGradient(solver->get_cellPressureField().get_internal(), solver->get_facePressureField().get_internal());

    // Assert
    ASSERT_EQ(pressureGradientField.size(), solver->get_mesh()->get_faces().size());
    for (int face=0 ; face<solver->get_mesh()->get_faces().size() ; face++){
        ASSERT_DOUBLE_EQ(pressureGradientField[face][0], 0.0);
        ASSERT_DOUBLE_EQ(pressureGradientField[face][1], 0.0);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Now we'll set once cell non-zero and test

    // Arrange
    std::vector<double> testPressureInternal = solver->get_cellPressureField().get_internal();
    testPressureInternal[3] = 1.0;
    UTILITIES::field<double> testPressureField(solver->get_mesh(),testPressureInternal);
    solver->set_cellPressureField(testPressureField);

    // Act
    const std::vector<MATH::Vector> perturbedPressureGradientField = 
        solver->computeFacePressureGradient(solver->get_cellPressureField().get_internal(), solver->get_facePressureField().get_internal());

    // Assert
    for (int face=0 ; face<solver->get_mesh()->get_faces().size() ; face++){
        ASSERT_DOUBLE_EQ(perturbedPressureGradientField[face][0], testFacePressureGradients[face][0]);
        ASSERT_DOUBLE_EQ(perturbedPressureGradientField[face][1], testFacePressureGradients[face][1]);
    }
}


// * * * * * * * * * * * * * *  test cell pressure gradients * * * * * * * * * * * * * * * //
TEST_F(solver_test, testCellPressureGradients)
{
    // Arrange

    // Act
    const std::vector<MATH::Vector> pressureGradientField = solver->computeCellPressureGradient(solver->get_facePressureField().get_internal());

    // Assert
    ASSERT_EQ(pressureGradientField.size(), solver->get_mesh()->get_elements().size());
    for (int face=0 ; face<solver->get_mesh()->get_elements().size() ; face++){
        ASSERT_DOUBLE_EQ(pressureGradientField[face][0], 0.0);
        ASSERT_DOUBLE_EQ(pressureGradientField[face][1], 0.0);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Now we'll set once cell non-zero and test

    // Arrange
    std::vector<double> testPressureInternal = solver->get_facePressureField().get_internal();
    testPressureInternal[2] = 1.0;
    testPressureInternal[9] = 1.0;
    UTILITIES::field<double> testPressureField(solver->get_mesh(),testPressureInternal);
    solver->set_facePressureField(testPressureField);

    // Act
    const std::vector<MATH::Vector> perturbedPressureGradientField = solver->computeCellPressureGradient(solver->get_facePressureField().get_internal());

    // Assert
    for (int face=0 ; face<solver->get_mesh()->get_elements().size() ; face++){
        ASSERT_DOUBLE_EQ(perturbedPressureGradientField[face][0], testCellPressureGradients[face][0]);
        ASSERT_DOUBLE_EQ(perturbedPressureGradientField[face][1], testCellPressureGradients[face][1]);
    }
}




// * * * * * * * * * * * * * *  test node value calculation * * * * * * * * * * * * * * * //
TEST_F(solver_test, testNodeValues)
{
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Trivial double case

    // Arrange
    BOUNDARIES::viscousWallBC testWallLower(solver,"lower");
    BOUNDARIES::viscousWallBC testWallUpper(solver,"upper");
    solver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(testWallLower));
    solver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(testWallUpper));

    // Act
    std::cout << "Computing nodal values...";
    const std::vector<double> nodeValues = solver->computeNodalScalar(solver->get_cellPressureField());
    std::cout << " done!" << std::endl;

    // Assert
    for (int node=0 ; node<solver->get_mesh()->get_nodes().size() ; node++){
        ASSERT_DOUBLE_EQ(nodeValues[node], 0.0);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Trivial Vector case

    // Arrange

    // Act
    const std::vector<MATH::Vector> nodeValuesVector = solver->computeNodalVector(solver->get_cellVelocityField());

    // Assert
    for (int node=0 ; node<solver->get_mesh()->get_nodes().size() ; node++){
        ASSERT_DOUBLE_EQ(nodeValuesVector[node][0], 0.0);
        ASSERT_DOUBLE_EQ(nodeValuesVector[node][1], 0.0);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Non-Trivial double case

    // Arrange
    std::vector<double> testPressureInternal = solver->get_cellPressureField().get_internal();
    testPressureInternal[3] = 1.0;
    testPressureInternal[4] = 1.0;
    UTILITIES::field<double> testPressureField(solver->get_mesh(),testPressureInternal,UTILITIES::fieldTypeEnum::PRESSURE);
    solver->set_cellPressureField(testPressureField);

    // Act
    std::cout << "Computing nodal values for perturbed pressure field..." << std::endl;
    const std::vector<double> perturbedNodeValues = solver->computeNodalScalar(solver->get_cellPressureField());

    // Assert
    for (int node=0 ; node<solver->get_mesh()->get_nodes().size() ; node++){
        // std::cout << perturbedNodeValues[node] << std::endl;
        // ASSERT_DOUBLE_EQ(perturbedNodeValues[node], 1.0);
    }
}


// * * * * * * * * * * * * * *  test face pressure calculations * * * * * * * * * * * * * * * //
TEST_F(solver_test, testPressureFaces)
{
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Trivial case
    BOUNDARIES::viscousWallBC testWallLower(solver,"lower");
    BOUNDARIES::viscousWallBC testWallUpper(solver,"upper");
    solver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(testWallLower));
    solver->setBoundaryCondition(std::make_shared<BOUNDARIES::viscousWallBC>(testWallUpper));

    // Arrange

    // Act
    std::vector<double> facePressure = solver->computeFacePressure(solver->get_cellPressureField().get_internal());

    // Assert
    for (int face=0 ; face<solver->get_mesh()->get_faces().size() ; face++){
        ASSERT_DOUBLE_EQ(facePressure[face], 0.0);
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Non-trivial case

    // Arrange
    std::vector<double> testPressureInternal = solver->get_cellPressureField().get_internal();
    testPressureInternal[3] = 2.0;
    testPressureInternal[4] = 1.0;
    UTILITIES::field<double> testPressureField(solver->get_mesh(),testPressureInternal);
    solver->set_cellPressureField(testPressureField);

    // Act
    facePressure = solver->computeFacePressure(solver->get_cellPressureField().get_internal());

    // Assert
    for (int face=0 ; face<solver->get_mesh()->get_faces().size() ; face++){
        ASSERT_DOUBLE_EQ(facePressure[face], testFacePressure[face]);
    }
}





