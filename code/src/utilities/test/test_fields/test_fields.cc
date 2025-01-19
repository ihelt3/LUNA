/*------------------------------------------------------------------------*\
**  
**  @file:      test_fields.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit tests for fields class
**
\*------------------------------------------------------------------------*/

#include <vector>
#include <filesystem>
#include <memory>

#include "gtest/gtest.h"

#include "fields.hh"
#include "read_su2.hh"

class test_fields : public ::testing::Test 
{
protected:
    // Member Data
    std::unique_ptr<UTILITIES::field<std::vector<double>>> test_vec_field;
    std::unique_ptr<UTILITIES::field<MATH::Vector>> test_Vec_field;
    std::unique_ptr<UTILITIES::field<double>> test_scalar_field;

    // Node Coordinates
    std::vector<std::vector<double>> coordinates = {
        {0.0,0.0}, {0.0,1.0}, {0.0,2.0}, {1.0,0.0}, {1.0,1.0}, {1.0,2.0}
    };
    
    // Element Nodes
    std::vector<std::vector<int>> element_nodes = {
        {0,1,3}, {1,3,4}, {1,2,4,5}
    };

    // Scalar Internal Field
    std::vector<double> scalar_field = {0.0, 0.0, 0.0};

    // Vector Internal Field
    std::vector<std::vector<double>> vector_field = {{0.0, 1.0, 2.0} , {0.0, 1.0, 2.0} , {0.0, 1.0, 2.0}};

    // MATH::Vector Internal Field
    MATH::Vector vi = MATH::Vector(std::vector<double>{0.0, 1.0, 2.0});

    void SetUp() override
    {
        // Define mesh path
        std::filesystem::path meshPath(MESH_DIR "/rectangle.su2"); // NOTE: MESH_DIR is a compile definition defined in CMakeLists.txt

        // Read mesh
        std::cout << "reading mesh..." << std::endl;
        MESH::read_su2 mesh(meshPath);
        std::cout << "Getting mesh pointer..." << std::endl;
        std::shared_ptr<MESH::mesh> mesh_ptr = std::make_shared<MESH::mesh>(mesh.get_mesh());

        // Initialize fields
        std::vector<double> vec_initial_value = {0.0, 1.0, 2.0};
        std::cout << " Assigning vector field:" << std::endl;
        test_vec_field = std::make_unique<UTILITIES::field<std::vector<double>>>(mesh_ptr, vec_initial_value, "cell");

        std::cout << "Assigning scalar field" << std::endl;
        test_scalar_field = std::make_unique<UTILITIES::field<double>>(mesh_ptr, 0.0, "cell");

        std::cout << "Assigning MATH::Vector field" << std::endl;
        test_Vec_field = std::make_unique<UTILITIES::field<MATH::Vector>>(mesh_ptr, vi, "cell");
        
        // Initialize test_scalar_field if needed
        // double scalar_initial_value = 0.0;  // Example initialization
        // test_scalar_field = field<double>(mesh.get_mesh(), scalar_initial_value);
    }
};

// Test that the field object actually contains the mesh
TEST_F(test_fields, test_mesh_read)
{
    // Arrange
    const auto& fieldi = *test_vec_field;

    // Act
    auto nodes = fieldi.get_mesh().get_nodes();
    auto elements = fieldi.get_mesh().get_elements();

    // Assert
    ASSERT_EQ(nodes.size(), 6);
    for (int node=0 ; node<nodes.size() ; node++){
        ASSERT_DOUBLE_EQ(nodes[node].get_coordinates()[0], coordinates[node][0]);
        ASSERT_DOUBLE_EQ(nodes[node].get_coordinates()[1], coordinates[node][1]);
    }
    ASSERT_EQ(elements.size(), 3);
    for (int element=0 ; element<elements.size() ; element++){
        for (int node=0 ; node<elements[element].get_nodes().size() ; node++){
            ASSERT_EQ(elements[element].get_nodes()[node].get_id(), element_nodes[element][node]);
        }
    }
    std::cout << "test_mesh_read passed" << std::endl;
};

// Test assigning a scalar field
TEST_F(test_fields, test_scalar_field)
{
    // Arrange
    const auto& fieldi = *test_scalar_field;

    // Act
    auto internal = fieldi.get_internal();

    // Assert
    for (int element=0 ; element<internal.size() ; element++){
        ASSERT_DOUBLE_EQ(internal[element], scalar_field[element]);
    }   
}

// Test assigning a vector field
TEST_F(test_fields, test_vector_field)
{
    // Arrange
    const auto& fieldi = *test_vec_field;

    // Act
    auto internal = fieldi.get_internal();

    // Assert
    for (int element=0 ; element<internal.size() ; element++){
        for (int i=0 ; i<internal[element].size() ; i++){
            ASSERT_DOUBLE_EQ(internal[element][i], vector_field[element][i]);
        }
    }
}

// Test vector field residual
TEST_F(test_fields, test_stdvector_residuals)
{
    std::cout << "testing std::vector residuals" << std::endl;
    // Arrange
    auto& fieldi = *test_vec_field;
    fieldi.set_internal(std::vector<std::vector<double>>{{1.0, 2.0, 3.0} , {1.0, 2.0, 3.0} , {1.0, 2.0, 3.0}});

    // Act
    auto resid = fieldi.get_residual();

    // Assert
    ASSERT_DOUBLE_EQ(resid[0], 3.0);
    ASSERT_DOUBLE_EQ(resid[1], 3.0);
    ASSERT_DOUBLE_EQ(resid[2], 3.0);
}

// Test scalar field residual
TEST_F(test_fields, test_scalar_residuals)
{
    std::cout << "testing scalar residuals" << std::endl;
    // Arrange
    auto& fieldi = *test_scalar_field;
    fieldi.set_internal({1.0, 2.0, 3.0});

    // Act
    auto resid = fieldi.get_residual();

    // Assert
    ASSERT_DOUBLE_EQ(resid, 6.0);
}

// Test MATH::Vector field residual
TEST_F(test_fields, test_MATHVector_residuals)
{
    std::cout << "testing MATH::Vector field residuals" << std::endl;
    // Arrange
    auto& fieldi = *test_Vec_field;
    MATH::Vector v2(std::vector<double>{2.0,2.0,2.0});
    fieldi.set_internal({v2, v2, v2});

    // Act
    auto resid = fieldi.get_residual();

    // Assert
    ASSERT_DOUBLE_EQ(resid[0], 6.0);
    ASSERT_DOUBLE_EQ(resid[1], 3.0);
    ASSERT_DOUBLE_EQ(resid[2], 0.0);
}



