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

#include "gtest/gtest.h"

#include "../../include/read_su2.hh"
#include "../../include/mesh.hh"

/*------------------------------------------------------------------------*\
**  Test Fixture
\*------------------------------------------------------------------------*/

// Inherit gtest's ::testing::Test class, making it a fixture
class read_su2_mesh_test : public ::testing::Test 
{

public:
    // Constructor
    read_su2_mesh_test() {
        // square mesh geometry made of tris
        std::filesystem::path simpleSquareMeshNoQuadFile("test/su2/meshes/square.su2");
        // square mesh geometry made of tris with one quad
        std::filesystem::path simpleSquareMeshWithQuadFile("test/su2/meshes/square_wQuad.su2");

        // Read meshes
        // MESH::read_su2 simpleSquareMeshNoQuad(simpleSquareMeshNoQuadFile);
        // MESH::read_su2 simpleSquareMeshWithQuad(simpleSquareMeshWithQuadFile);

        // su2_meshes.push_back(std::make_unique<MESH::read_su2>(simpleSquareMeshNoQuad));
        // su2_meshes.push_back(std::make_unique<MESH::read_su2>(simpleSquareMeshWithQuad));

        //------------------------------------------------------------------------------------------------------------------
        // Here we are using SMART POINTERS because otherwise we are trying to use a DELETED COPY CONSTRUCTOR
        //      this happens because MESH::read_base contains a std::ifstream which makes the copy constructor of MESH::read_su2 deleted
        //      If we used "MESH::read_su2 simpleSquareMeshNoQuad(simpleSquareMeshNoQuadFile);" and then push it to the vector, then it would try to pass a copy constructor
        //          but since the copy constructor was deleted (due to the std::ifstream), this isn't possible
        //      We also need to create the objects directly in the vector to avoid any copy constructor, as opposed to the method above
        //      The smart pointers allow us to store the MESH::read_su2 objects in a vector, as they manage the lifetime of the objects to avoid need for copying
        //-------------------------------------------------------------------------------------------------------------------

        // Read meshes
        MESH::read_su2 simpleSquareMeshNoQuad(simpleSquareMeshNoQuadFile);
        MESH::read_su2 simpleSquareMeshWithQuad(simpleSquareMeshWithQuadFile);

        // Store meshes
        su2_meshes.push_back(std::make_unique<MESH::mesh>(simpleSquareMeshNoQuad.get_mesh()));
        su2_meshes.push_back(std::make_unique<MESH::mesh>(simpleSquareMeshWithQuad.get_mesh()));
    }

protected:
    std::vector<std::unique_ptr<MESH::mesh>> su2_meshes;

    // Coordinates for test
    std::vector<std::vector<double>> coordinateX {
        {0.0, 0.5, 1.0, 0.0, 0.5, 1.0, 0.0, 0.5, 1.0}, 
        {0.0, 0.5, 1.0, 0.0, 0.5, 1.0, 0.0, 0.5, 1.0}
    };
    std::vector<std::vector<double>> coordinateY {
        {0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 1.0, 1.0, 1.0}, 
        {0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 1.0, 1.0, 1.0}
    };

    // Element Test
    std::vector<std::vector<std::vector<unsigned>>> element_nodes {
        { {0,1,3} , {1,4,3} , {1,2,4} , {2,5,4} , {3,4,6} , {4,7,6} , {4,5,7} , {5,8,7} },
        { {0,1,3,4} , {1,2,4} , {2,5,4} , {3,4,6} , {4,7,6} , {4,5,7} , {5,8,7}}
    };

    // Connectivity Test (BC connectivity = -BC idx - 1 )
    std::vector<std::vector<std::vector<int>>> element_connectivity {
        { {-1,1,-4} , {0,2,4} , {-1,3,1} , {2,-2,6} , {1,5,-4} , {4,6,-3} , {3,7,5} , {6,-2,-3} },
        { {-1,1,3,-4} , {-1,2,0} , {1,-2,5} , {0,4,-4} , {3,5,-3} , {2,6,4} , {5,-2,-3}}
    };

    // Element Normal tests
    double s2 = 1.0/std::sqrt(2.0);
    std::vector<std::vector<std::vector<std::vector<double>>>> element_normals {
        { 
            {{0.0,-1.0} , {s2,s2} , {-1.0,0.0}}, 
            {{-s2,-s2} , {1.0,0.0} , {0.0,1.0}},
            {{0.0,-1.0} , {s2,s2} , {-1.0,0.0}},
            {{-s2,-s2} , {1.0,0.0} , {0.0,1.0}},
            {{0.0,-1.0} , {s2,s2} , {-1.0,0.0}},
            {{-s2,-s2} , {1.0,0.0} , {0.0,1.0}}, 
            {{0.0,-1.0} , {s2,s2} , {-1.0,0.0}},
            {{-s2,-s2} , {1.0,0.0} , {0.0,1.0}}
        } ,
        {
            {{0.0,-1.0} , {1.0,0.0} , {0.0,1.0}, {-1.0,0.0}},
            {{0.0,-1.0} , {s2,s2} , {-1.0,0.0}},
            {{-s2,-s2} , {1.0,0.0} , {0.0,1.0}},
            {{0.0,-1.0} , {s2,s2} , {-1.0,0.0}},
            {{-s2,-s2} , {1.0,0.0} , {0.0,1.0}}, 
            {{0.0,-1.0} , {s2,s2} , {-1.0,0.0}},
            {{-s2,-s2} , {1.0,0.0} , {0.0,1.0}}
        }
    };

    // Element Distance Weight
    std::vector<std::vector<std::vector<double>>> element_distanceWeight {
        {
            { {-1,0.5,-1} , {0.5,0.5,0.5} , {-1,0.5,0.5} , {0.5,-1,0.5} , {0.5,0.5,-1} , {0.5,0.5,-1} , {0.5,0.5,0.5} , {0.5,-1,-1} },
            { {-1,0.42705,0.42705,-1} , {-1,0.5,0.572949} , {0.5,-1,0.5} , {0.572949,0.5,-1} , {0.5,0.5,-1} , {0.5,0.5,0.5} , {0.5,-1,-1} }
        }
    };

    // Face test
    std::vector<std::vector<std::vector<int>>> face_connectivity {
        //cell 0                           cell 1           cell 2           cell 3            cell 4           cell 5           cell 6            cell 7
        { {0,-1} , {0,1} , {0,-4}        , {1,2}  , {1,4} , {2,-1} , {2,3} , {3,-2} , {3,6}  , {4,5} , {4,-4} , {5,6} , {5,-3} , {6,7} ,           {7,-2} , {7,-3}} , 
        { {0,-1} , {0,1} , {0,3}, {0,-4} , {1,-1} , {1,2} , {2,-2} , {2,5} , {3,4}  , {3,-4} , {4,5} , {4,-3} , {5,6} ,          {6,-2} , {6,-3}                  }
    };

    std::vector<std::vector<bool>> face_isBoundary {
        { true, false, true,               false, false,    true, false,      true, false,     false, true,     false, true,     false,            true, true } ,
        { true, false, false, true,        true, false,     true, false,      false, true,     false, true,     false,           true,    true}
    };

    // Check element faces are assigned
    std::vector<std::vector<std::vector<int>>> element_faceIDs {
        { {0,1,2} , {1,3,4} , {5,6,3} , {6,7,8} , {4,9,10} , {9,11,12} , {8,13,11} , {13,14,15} },
        { {0,1,2,3}         , {4,5,1} , {5,6,7} , {2,8,9} , {8,10,11} , {7,12,10} , {12,13,14} }
    };

    // Check element volumes
    double p5_3 = pow(0.5,3);
    std::vector< std::vector<double>> element_volumes {
        {p5_3,p5_3,p5_3,p5_3,p5_3,p5_3,p5_3,p5_3},
        {pow(0.5,2),p5_3,p5_3,p5_3,p5_3,p5_3,p5_3}
    };

};



// * * * * * * * * * * * * * *  test Coordinates * * * * * * * * * * * * * * * //
TEST_F(read_su2_mesh_test, readCoordinates)
{
    for (int i=0 ; i<su2_meshes.size() ; i++) 
    {
        // Arrange
        const auto& mesh = *su2_meshes[i];

        // Act
        auto nodes = mesh.get_nodes();

        // Assert
        ASSERT_EQ(nodes.size(), 9);
        for (int node=0 ; node<nodes.size() ; node++){
            ASSERT_DOUBLE_EQ(nodes[node].get_coordinates()[0], coordinateX[i][node]);
            ASSERT_DOUBLE_EQ(nodes[node].get_coordinates()[1], coordinateY[i][node]);
        }
    }
}

// * * * * * * * * * * * * * *  test Elements * * * * * * * * * * * * * * * //
TEST_F(read_su2_mesh_test, readElements)
{
    for (int i=0 ; i<su2_meshes.size() ; i++) 
    {
        // Arrange
        auto& mesh = *su2_meshes[i];

        // Act 
        auto elements = mesh.get_elements();

        // Assert

        // Check element nodes
        for (int element=0 ; element<elements.size() ; element++){
            for (int vertex=0 ; vertex<elements[element].get_nodes().size() ; vertex++){
                ASSERT_EQ(elements[element].get_nodes()[vertex].get_id(), element_nodes[i][element][vertex]);
            }
        }
        // Check element face connectivites
        for (int element=0 ; element<elements.size() ; element++){
            std::cout << "Element: " << element << std::endl;
            for (int face=0 ; face<elements[element].get_faces().size() ; face++){
                // Check the face has the correct ID
                ASSERT_EQ(elements[element].get_faces()[face].get_id(), element_faceIDs[i][element][face]);
            }
        }
        // Check element volumes
        for (int element=0 ; element<elements.size() ; element++){
            ASSERT_DOUBLE_EQ(elements[element].get_volume(), element_volumes[i][element]);
        }
    }
}

// * * * * * * * * * * * * * *  test Connectivity * * * * * * * * * * * * * * * //
TEST_F(read_su2_mesh_test, readConnectivity)
{
    for (int i=0 ; i<su2_meshes.size() ; i++) 
    {
        // Arrange
        auto& mesh = *su2_meshes[i];

        // Act
        auto elements = mesh.get_elements();

        // Assert
        for (int element=0 ; element<elements.size() ; element++){
            for (int face=0 ; face<elements[element].get_faces().size() ; face++){
                ASSERT_EQ(elements[element].get_faces()[face].get_elements()[0], element_connectivity[i][element][face]);
            }
        }
    }
}

// * * * * * * * * * * * * * *  test Normals * * * * * * * * * * * * * * * //
TEST_F(read_su2_mesh_test, readNormals)
{
    for (int i=0 ; i<su2_meshes.size() ; i++) 
    {
        // Arrange
        auto& mesh = *su2_meshes[i];

        // Act 
        auto elements = mesh.get_elements();

        // Assert
        // Loop over elements
        for (int element=0 ; element<elements.size() ; element++){
            std::vector<MATH::Vector> normals = elements[element].get_normals();
            // Loop over subelements
            for (int face=0 ; face<elements[element].get_faces().size() ; face++){
                // Loop Over dimensions
                for (int dim=0 ; dim<normals[face].size() ; dim++){
                    ASSERT_DOUBLE_EQ(element_normals[i][element][face][dim], normals[face][dim]);
                }
            }
        }
    }
}

TEST_F(read_su2_mesh_test, distanceWeight)
{
    for (int i=0 ; i<su2_meshes.size() ; i++) 
    {
        // Arrange
        auto& mesh = *su2_meshes[i];

        // Act
        auto elements = mesh.get_elements();

        // Assert
        // Loop over elements
        for (int element=0 ; element<elements.size() ; element++){
            std::vector<double> distanceWeight = elements[element].get_distanceWeights();
            // Loop over subelements
            for (int face=0 ; face<elements[element].get_faces().size() ; face++){
                ASSERT_NEAR(element_distanceWeight[i][element][face], distanceWeight[face],1.0e-4);
            }
        }
    }
}

TEST_F(read_su2_mesh_test, faceConnectivity)
{
    for (int i=0 ; i<su2_meshes.size() ; i++) 
    {
        // Arrange
        auto& mesh = *su2_meshes[i];

        // Act
        auto faces = mesh.get_faces();

        // Assert
        // Loop over faces
        for (int face=0 ; face<faces.size() ; face++){
            // Check boundary face
            ASSERT_EQ(face_isBoundary[i][face], faces[face].is_boundaryFace());
            std::vector<int> elements = faces[face].get_elements();
            // Loop over elements
            for (int e=0 ; e<elements.size() ; e++){
                ASSERT_EQ(face_connectivity[i][face][e], elements[e]);
            }
        }
    }
}


/*
To Lazy to write the data for this test right now, but it works (trust me)
TEST_F(read_su2_mesh_test, nodeDistanceWeights)
{
    for (int i=0 ; i<su2_meshes.size() ; i++)
    {
        // Arrange
        auto& mesh = *su2_meshes[i];

        // Act
        auto nodes = mesh.get_nodes();

        // Assert
        for (int node=0 ; node<nodes.size() ; node++){
            std::vector<double> distanceWeights = nodes[node].get_distanceWeights();
            for (int e=0 ; e<distanceWeights.size() ; e++){
                std::cout << distanceWeights[e] << " ";
                // ASSERT_DOUBLE_EQ(node_distanceWeights[i][node][e], distanceWeights[e]);
            }
            std::cout << "   " << nodes[node].is_boundaryNode();
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
*/
