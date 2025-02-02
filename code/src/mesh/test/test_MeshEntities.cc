/*------------------------------------------------------------------------*\
**  
**  @file:      test_MeshEntities.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit tests for MeshEntities
**
\*------------------------------------------------------------------------*/

#include <filesystem>
#include <vector>

#include "gtest/gtest.h"

#include "MeshEntities.hh"
#include "mesh.hh"




/*------------------------------------------------------------------------*\
**  testing Nodes
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * distance weights * * * * * * * * * * * * * * //
TEST(test_nodes, testNodeDistanceWeights) 
{
    /* Arrange */
    // Make Node
    std::cout << "INSTANTIATING NODES" << std::endl;
    MESH::node n1(1, MATH::Vector(std::vector<double>{0.0,0.0}));
    MESH::node n2(2, MATH::Vector(std::vector<double>{1.0,0.0}));
    MESH::node n3(3, MATH::Vector(std::vector<double>{0.0,1.0}));
    MESH::node n4(4, MATH::Vector(std::vector<double>{1.0,1.0}));
    std::shared_ptr<MESH::node> pn1 = std::make_shared<MESH::node>(n1);
    std::shared_ptr<MESH::node> pn2 = std::make_shared<MESH::node>(n2);
    std::shared_ptr<MESH::node> pn3 = std::make_shared<MESH::node>(n3);
    std::shared_ptr<MESH::node> pn4 = std::make_shared<MESH::node>(n4);
    // Make Elements
    std::cout << "INSTANTIATING ELEMENTS" << std::endl;
    std::vector<std::weak_ptr<MESH::node>> ptr1 = {pn1,pn2,pn3};
    std::vector<std::weak_ptr<MESH::node>> ptr2 = {pn2,pn3,pn4};
    MESH::element e1(1, elementTypeEnum::TRIANGLE, ptr1);
    MESH::element e2(2, elementTypeEnum::TRIANGLE, ptr2);
    std::shared_ptr<MESH::element> pe1 = std::make_shared<MESH::element>(e1);
    std::shared_ptr<MESH::element> pe2 = std::make_shared<MESH::element>(e2);
    std::vector<std::weak_ptr<MESH::element>> element_ptrs = {pe1,pe2};
    // Add elements to node
    n2.set_elements(element_ptrs);

    /* Act */
    std::cout << "CALCULATING DISTANCE WEIGHTS" << std::endl;
    n2.calculateElementDistanceWeights();

    /* Assert */
    EXPECT_EQ(n2.get_distanceWeights()[0], 0.5);
    EXPECT_EQ(n2.get_distanceWeights()[1], 0.5);
}


// /*------------------------------------------------------------------------*\
// **  testing mesh entity
// \*------------------------------------------------------------------------*/

// // * * * * * * * * * * Instantiate with vector of nodes * * * * * * * * * * * //
// TEST(test_mesh_entity, testMeshEntityNodes)
// {
//     /* Arrange */
//     // Make Nodes
//     MESH::node n1(1, MATH::Vector(std::vector<double>{0.0,0.0}));
//     MESH::node n2(2, MATH::Vector(std::vector<double>{1.0,0.0}));
//     MESH::node n3(3, MATH::Vector(std::vector<double>{0.0,1.0}));    
    
//     /* Act */
//     MESH::mesh_entity me(1, elementTypeEnum::TRIANGLE, std::vector<MESH::node>{n1,n2,n3});
    
//     /* Assert */
//     EXPECT_EQ(me.get_id(), 1);
//     EXPECT_EQ(me.get_nodes()[0].get_id(), 1);
//     EXPECT_EQ(me.get_nodes()[1].get_id(), 2);
//     EXPECT_EQ(me.get_nodes()[2].get_id(), 3);
//     EXPECT_DOUBLE_EQ(me.get_volume(), 0.5);
//     EXPECT_DOUBLE_EQ(me.get_centroid()[0], 1.0/3.0);
//     EXPECT_DOUBLE_EQ(me.get_centroid()[1], 1.0/3.0);
// }

// // * * * * * * * * * * Instantiate with vector of faces * * * * * * * * * * * //
// TEST(test_mesh_entity, testMeshEntityFaces)
// {
//     /* Arrange */
//     // Make Nodes
//     MESH::node n1(1, MATH::Vector(std::vector<double>{0.0,0.0}));
//     MESH::node n2(2, MATH::Vector(std::vector<double>{1.0,0.0}));
//     MESH::node n3(3, MATH::Vector(std::vector<double>{0.0,1.0}));  
//     // Make Faces
//     MESH::face f1(1, elementTypeEnum::LINE, std::vector<MESH::node>{std::make_shared<MESH::node>(n1),std::make_shared<MESH::node>(n2)});  
//     MESH::face f2(1, elementTypeEnum::LINE, std::vector<MESH::node>{std::make_shared<MESH::node>(n3),std::make_shared<MESH::node>(n2)});  
//     MESH::face f3(1, elementTypeEnum::LINE, std::vector<MESH::node>{std::make_shared<MESH::node>(n1),std::make_shared<MESH::node>(n3)});  
    
//     /* Act */
//     MESH::mesh_entity me(1, elementTypeEnum::TRIANGLE, std::vector<MESH::face>{std::make_shared<MESH::face>(f1),std::make_shared<MESH::face>(f2),std::make_shared<MESH::face>(f3)}); //std::vector<MESH::face>{f1,f2,f3});
    
//     /* Assert */
//     EXPECT_EQ(me.get_id(), 1);
//     EXPECT_EQ(me.get_nodes()[0]->get_id(), 1);
//     EXPECT_EQ(me.get_nodes()[1]->get_id(), 2);
//     EXPECT_EQ(me.get_nodes()[2]->get_id(), 3);
//     EXPECT_DOUBLE_EQ(me.get_volume(), 0.5);
//     EXPECT_DOUBLE_EQ(me.get_centroid()[0], 1.0/3.0);
//     EXPECT_DOUBLE_EQ(me.get_centroid()[1], 1.0/3.0);
// }

// /*------------------------------------------------------------------------*\
// **  testing elements
// \*------------------------------------------------------------------------*/

// // * * * * * * * * * * Instantiate with vector of nodes * * * * * * * * * * * //
// TEST(test_element, testElementNodes)
// {
//     /* Arrange */
//     // Make Nodes
//     MESH::node n1(1, MATH::Vector(std::vector<double>{0.0,0.0}));
//     MESH::node n2(2, MATH::Vector(std::vector<double>{1.0,0.0}));
//     MESH::node n3(3, MATH::Vector(std::vector<double>{0.0,1.0}));    
    
//     /* Act */
//     MESH::element e(1, elementTypeEnum::TRIANGLE, std::vector<MESH::node>{n1,n2,n3});
    
//     /* Assert */
//     EXPECT_EQ(e.get_id(), 1);
//     EXPECT_EQ(e.get_nodes()[0].get_id(), 1);
//     EXPECT_EQ(e.get_nodes()[1].get_id(), 2);
//     EXPECT_EQ(e.get_nodes()[2].get_id(), 3);
//     // Check volume
//     EXPECT_DOUBLE_EQ(e.get_volume(), 0.5);
//     // Check centroid
//     EXPECT_DOUBLE_EQ(e.get_centroid()[0], 1.0/3.0);
//     EXPECT_DOUBLE_EQ(e.get_centroid()[1], 1.0/3.0);
//     // Check Normals
//     EXPECT_DOUBLE_EQ(e.get_faces()[0].get_normal()[0], 0.0);
//     EXPECT_DOUBLE_EQ(e.get_faces()[0].get_normal()[1], -1.0);
//     EXPECT_DOUBLE_EQ(e.get_faces()[1].get_normal()[0], pow(1.0/2.0,1.0/2.0));
//     EXPECT_DOUBLE_EQ(e.get_faces()[1].get_normal()[1], pow(1.0/2.0,1.0/2.0));
//     EXPECT_DOUBLE_EQ(e.get_faces()[2].get_normal()[0], -1.0);
//     EXPECT_DOUBLE_EQ(e.get_faces()[2].get_normal()[1], 0.0);
// }

// // * * * * * * * * * * Instantiate with vector of faces * * * * * * * * * * * //
// TEST(test_element, testElementFaces)
// {
//     /* Arrange */
//     // Make Nodes
//     MESH::node n1(1, MATH::Vector(std::vector<double>{0.0,0.0}));
//     MESH::node n2(2, MATH::Vector(std::vector<double>{1.0,0.0}));
//     MESH::node n3(3, MATH::Vector(std::vector<double>{0.0,1.0}));  
//     // Make Faces
//     MESH::face f1(1, elementTypeEnum::LINE, std::vector<MESH::node>{n1,n2});  
//     MESH::face f2(1, elementTypeEnum::LINE, std::vector<MESH::node>{n3,n2});  
//     MESH::face f3(1, elementTypeEnum::LINE, std::vector<MESH::node>{n1,n3});  
    
//     /* Act */
//     MESH::element e(1, elementTypeEnum::TRIANGLE, std::vector<MESH::face>{f1,f2,f3});
    
//     /* Assert */
//     EXPECT_EQ(e.get_id(), 1);
//     EXPECT_EQ(e.get_nodes()[0].get_id(), 1);
//     EXPECT_EQ(e.get_nodes()[1].get_id(), 2);
//     EXPECT_EQ(e.get_nodes()[2].get_id(), 3);
//     // Check volume
//     EXPECT_DOUBLE_EQ(e.get_volume(), 0.5);
//     // Check centroid
//     EXPECT_DOUBLE_EQ(e.get_centroid()[0], 1.0/3.0);
//     EXPECT_DOUBLE_EQ(e.get_centroid()[1], 1.0/3.0);
//     // Check Normals
//     EXPECT_DOUBLE_EQ(e.get_faces()[0].get_normal()[0], 0.0);
//     EXPECT_DOUBLE_EQ(e.get_faces()[0].get_normal()[1], -1.0);
//     EXPECT_DOUBLE_EQ(e.get_faces()[1].get_normal()[0], pow(1.0/2.0,1.0/2.0));
//     EXPECT_DOUBLE_EQ(e.get_faces()[1].get_normal()[1], pow(1.0/2.0,1.0/2.0));
//     EXPECT_DOUBLE_EQ(e.get_faces()[2].get_normal()[0], -1.0);
//     EXPECT_DOUBLE_EQ(e.get_faces()[2].get_normal()[1], 0.0);
// }



    