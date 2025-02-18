/*------------------------------------------------------------------------*\
**  
**  @file:      test_mesh.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit tests for reading su2 file
**
\*------------------------------------------------------------------------*/

#include <filesystem>
#include <vector>

#include "gtest/gtest.h"

#include "read_su2.hh"
#include "mesh.hh"

/*------------------------------------------------------------------------*\
**  Test Fixture
\*------------------------------------------------------------------------*/

// Inherit gtest's ::testing::Test class, making it a fixture
class mesh_test : public ::testing::Test 
{

public:
    // Constructor
    mesh_test() {
        
        // square mesh geometry made of tris
        std::filesystem::path simpleSquareMeshNoQuadFile(SU2_MESH_DIR "/su2/square.su2"); // NOTE: SU2_MESH_DIR is a compile definition defined in CMakeLists.txt
        // square mesh geometry made of tris with one quad
        std::filesystem::path simpleSquareMeshWithQuadFile(SU2_MESH_DIR "/su2/square_wQuad.su2"); // NOTE: SU2_MESH_DIR is a compile definition defined in CMakeLists.txt

        // Read meshes
        MESH::read_su2 simpleSquareMeshNoQuad(simpleSquareMeshNoQuadFile);
        MESH::read_su2 simpleSquareMeshWithQuad(simpleSquareMeshWithQuadFile);

        // Store meshes
        su2_meshes.push_back(std::make_unique<MESH::mesh>(simpleSquareMeshNoQuad.get_mesh()));
        su2_meshes.push_back(std::make_unique<MESH::mesh>(simpleSquareMeshWithQuad.get_mesh()));
    }

protected:
    std::vector<std::unique_ptr<MESH::mesh>> su2_meshes;

    double s2 = sqrt(2.0);
    // Face normal deltas
    std::vector<std::vector<double>> testFaceNormalDeltas {
        { 1.0/6.0, 1.0/pow(18.0,0.5), 1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0, 1.0/pow(18.0,0.5), 1.0/6.0, 1.0/3.0, 1.0/pow(18.0,0.5), 1.0/6.0, 1.0/3.0, 1.0/6.0, 1.0/pow(18.0,0.5), 1.0/6.0, 1.0/6.0, },
        { 0.25, 5.0/12.0, 5.0/12.0, 0.25, 1.0/6.0, 1.0/pow(18.0,0.5), 1.0/6.0, 1.0/3.0, 1.0/pow(18.0,0.5), 1.0/6.0, 1.0/3.0, 1.0/6.0, 1.0/pow(18.0,0.5), 1.0/6.0, 1.0/6.0 }
    };
};


// FINISH THIS
TEST_F(mesh_test, readFaceNormalDeltas)
{
    for (int i=0 ; i<su2_meshes.size() ; i++) 
    {
        // Arrange
        const auto& mesh = *su2_meshes[i];

        // Act
        std::vector<double> faceNormalDeltas = mesh.get_faceNormalDeltas();

        // Assert
        ASSERT_EQ(faceNormalDeltas.size(), mesh.get_faces().size());
        for (int f=0 ; f<mesh.get_faces().size() ; f++){
            ASSERT_DOUBLE_EQ(faceNormalDeltas[f], testFaceNormalDeltas[i][f]);
            ASSERT_DOUBLE_EQ(faceNormalDeltas[f], testFaceNormalDeltas[i][f]);
        }
    }
}
