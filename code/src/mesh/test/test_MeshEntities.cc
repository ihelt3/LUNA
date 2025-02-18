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
#include <memory>

#include "gtest/gtest.h"

#include "Vector.hh"
#include "MeshEntities.hh"
#include "mesh.hh"

class meshEntities_test : public ::testing::Test 
{

public:
    // Constructor: Make a simple square which is divided into two triangles
    meshEntities_test() 
    :
        pn1(std::make_shared<MESH::node>(MESH::node(1, MATH::Vector(std::vector<double>{0.0,0.0})))),
        pn2(std::make_shared<MESH::node>(MESH::node(2, MATH::Vector(std::vector<double>{1.0,0.0})))),
        pn3(std::make_shared<MESH::node>(MESH::node(3, MATH::Vector(std::vector<double>{0.0,1.0})))),
        pn4(std::make_shared<MESH::node>(MESH::node(4, MATH::Vector(std::vector<double>{1.0,1.0})))),
        pf1(std::make_shared<MESH::face>(MESH::face(1, elementTypeEnum::LINE, std::vector<std::weak_ptr<MESH::node>>{pn1,pn2}, true))),
        pf2(std::make_shared<MESH::face>(MESH::face(2, elementTypeEnum::LINE, std::vector<std::weak_ptr<MESH::node>>{pn2,pn3}, false))),
        pf3(std::make_shared<MESH::face>(MESH::face(3, elementTypeEnum::LINE, std::vector<std::weak_ptr<MESH::node>>{pn1,pn3}, true))),
        pe1(std::make_shared<MESH::element>(MESH::element(1, elementTypeEnum::TRIANGLE, std::vector<std::weak_ptr<MESH::node>>{pn1,pn2,pn3}))),
        pe2(std::make_shared<MESH::element>(MESH::element(2, elementTypeEnum::TRIANGLE, std::vector<std::weak_ptr<MESH::node>>{pn2,pn3,pn4})))
    {
        // Add owners to faces
        pf1->set_owner(pe1);
        pf2->set_owner(pe1);
        pf2->set_neighbor(pe2);
        std::cout << pf2->get_neighbor()->get_id() << std::endl;
        pf3->set_owner(pe1);

        // Add faces to elements
        pe1->add_face(pf1);
        pe1->add_face(pf2);
        pe1->add_face(pf3);
    };
    

protected:
    // Node data
    std::shared_ptr<MESH::node> pn1;
    std::shared_ptr<MESH::node> pn2;
    std::shared_ptr<MESH::node> pn3;
    std::shared_ptr<MESH::node> pn4;
    std::shared_ptr<MESH::face> pf1;
    std::shared_ptr<MESH::face> pf2;
    std::shared_ptr<MESH::face> pf3;
    std::shared_ptr<MESH::element> pe1;
    std::shared_ptr<MESH::element> pe2;
};



/*------------------------------------------------------------------------*\
**  testing Nodes
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * distance weights * * * * * * * * * * * * * * //
TEST_F(meshEntities_test, testNodeDistanceWeights) 
{
    /* Arrange */
    // Make Elements
    std::vector<std::weak_ptr<MESH::element>> element_ptrs = {this->pe1,this->pe2};
    // Add elements to node
    std::shared_ptr<MESH::node> pn2 = this->pn2;
    pn2->set_elements(element_ptrs);

    /* Act */
    pn2->calculateElementDistanceWeights();

    /* Assert */
    EXPECT_EQ(pn2->get_distanceWeights()[0], 0.5);
    EXPECT_EQ(pn2->get_distanceWeights()[1], 0.5);
}


/*------------------------------------------------------------------------*\
**  testing mesh entity
\*------------------------------------------------------------------------*/

// * * * * * * * * * * Instantiate with vector of nodes * * * * * * * * * * * //
TEST_F(meshEntities_test, testMeshEntityNodes)
{
    /* Arrange */
    std::vector<std::weak_ptr<MESH::node>> nodes = {this->pn1,this->pn2,this->pn3};

    /* Act */
    MESH::mesh_entity me(1, elementTypeEnum::TRIANGLE, nodes);
    
    /* Assert */
    EXPECT_EQ(me.get_id(), 1);
    EXPECT_EQ(me.get_nodes()[0]->get_id(), 1);
    EXPECT_EQ(me.get_nodes()[1]->get_id(), 2);
    EXPECT_EQ(me.get_nodes()[2]->get_id(), 3);
    EXPECT_DOUBLE_EQ(me.get_volume(), 0.5);
    EXPECT_DOUBLE_EQ(me.get_centroid()[0], 1.0/3.0);
    EXPECT_DOUBLE_EQ(me.get_centroid()[1], 1.0/3.0);
}

// * * * * * * * * * * Instantiate with vector of faces * * * * * * * * * * * //
TEST_F(meshEntities_test, testMeshEntityFaces)
{
    /* Arrange */ 
    
    /* Act */
    MESH::mesh_entity me(1, elementTypeEnum::TRIANGLE, std::vector<std::weak_ptr<MESH::face>>{this->pf1,this->pf2,this->pf3}); //std::vector<MESH::face>{f1,f2,f3});
    
    /* Assert */
    EXPECT_EQ(me.get_id(), 1);
    EXPECT_EQ(me.get_nodes()[0]->get_id(), 1);
    EXPECT_EQ(me.get_nodes()[1]->get_id(), 2);
    EXPECT_EQ(me.get_nodes()[2]->get_id(), 3);
    EXPECT_DOUBLE_EQ(me.get_volume(), 0.5);
    EXPECT_DOUBLE_EQ(me.get_centroid()[0], 1.0/3.0);
    EXPECT_DOUBLE_EQ(me.get_centroid()[1], 1.0/3.0);
}

/*------------------------------------------------------------------------*\
**  testing faces
\*------------------------------------------------------------------------*/

TEST_F(meshEntities_test, testFaces)
{
    /* Arrange */

    /* Act */

    /* Assert */
    // Testing IDs
    EXPECT_EQ(this->pf1->get_id(), 1);
    EXPECT_EQ(this->pf2->get_id(), 2);
    EXPECT_EQ(this->pf3->get_id(), 3);
    // Testing Areas
    EXPECT_DOUBLE_EQ(this->pf1->get_volume(), 1.0);
    EXPECT_DOUBLE_EQ(this->pf2->get_volume(), pow(2.0,1.0/2.0));
    EXPECT_DOUBLE_EQ(this->pf3->get_volume(), 1.0);
    // Testing Normals
    EXPECT_DOUBLE_EQ(this->pf1->get_normal()[0], 0.0);
    EXPECT_DOUBLE_EQ(this->pf1->get_normal()[1], -1.0);
    EXPECT_DOUBLE_EQ(this->pf2->get_normal()[0], pow(1.0/2.0,1.0/2.0));
    EXPECT_DOUBLE_EQ(this->pf2->get_normal()[1], pow(1.0/2.0,1.0/2.0));
    EXPECT_DOUBLE_EQ(this->pf3->get_normal()[0], -1.0);
    EXPECT_DOUBLE_EQ(this->pf3->get_normal()[1], 0.0);
}

/*------------------------------------------------------------------------*\
**  testing elements
\*------------------------------------------------------------------------*/

// * * * * * * * * * * Instantiate with vector of nodes * * * * * * * * * * * //
TEST_F(meshEntities_test, testElementNodes)
{
    /* Arrange */   
    MESH::element e(1, elementTypeEnum::TRIANGLE, std::vector<std::weak_ptr<MESH::node>>{this->pn1,this->pn2,this->pn3});
    e.add_face(this->pf1);
    e.add_face(this->pf2);
    e.add_face(this->pf3);

    /* Act */
    
    /* Assert */
    EXPECT_EQ(e.get_id(), 1);
    EXPECT_EQ(e.get_nodes()[0]->get_id(), 1);
    EXPECT_EQ(e.get_nodes()[1]->get_id(), 2);
    EXPECT_EQ(e.get_nodes()[2]->get_id(), 3);
    // Check volume
    EXPECT_DOUBLE_EQ(e.get_volume(), 0.5);
    // Check centroid
    EXPECT_DOUBLE_EQ(e.get_centroid()[0], 1.0/3.0);
    EXPECT_DOUBLE_EQ(e.get_centroid()[1], 1.0/3.0);
    // Check Normals
    e.calculateOutwardNormals();
    EXPECT_DOUBLE_EQ(e.get_normals()[0][0], 0.0);
    EXPECT_DOUBLE_EQ(e.get_normals()[0][1], -1.0);
    EXPECT_DOUBLE_EQ(e.get_normals()[1][0], pow(1.0/2.0,1.0/2.0));
    EXPECT_DOUBLE_EQ(e.get_normals()[1][1], pow(1.0/2.0,1.0/2.0));
    EXPECT_DOUBLE_EQ(e.get_normals()[2][0], -1.0);
    EXPECT_DOUBLE_EQ(e.get_normals()[2][1], 0.0);
}

// * * * * * * * * * * Instantiate with vector of faces * * * * * * * * * * * //
TEST_F(meshEntities_test, testElementFaces)
{
    /* Arrange */ 
    std::cout << "face 2 neighbor: " << this->pf2->get_neighbor()->get_id() << std::endl;
    MESH::element e(1, elementTypeEnum::TRIANGLE, std::vector<std::weak_ptr<MESH::face>>{this->pf1,this->pf2,this->pf3});
    std::cout << e.add_face(this->pf1) << std::endl;
    std::cout << e.add_face(this->pf2) << std::endl;
    std::cout << e.add_face(this->pf3) << std::endl;

    /* Act */
    
    
    /* Assert */
    EXPECT_EQ(e.get_id(), 1);
    EXPECT_EQ(e.get_nodes()[0]->get_id(), 1);
    EXPECT_EQ(e.get_nodes()[1]->get_id(), 2);
    EXPECT_EQ(e.get_nodes()[2]->get_id(), 3);
    // Check volume
    EXPECT_DOUBLE_EQ(e.get_volume(), 0.5);
    // Check centroid
    EXPECT_DOUBLE_EQ(e.get_centroid()[0], 1.0/3.0);
    EXPECT_DOUBLE_EQ(e.get_centroid()[1], 1.0/3.0);
    // Check Normals
    e.calculateOutwardNormals();
    EXPECT_DOUBLE_EQ(e.get_normals()[0][0], 0.0);
    EXPECT_DOUBLE_EQ(e.get_normals()[0][1], -1.0);
    EXPECT_DOUBLE_EQ(e.get_normals()[1][0], pow(1.0/2.0,1.0/2.0));
    EXPECT_DOUBLE_EQ(e.get_normals()[1][1], pow(1.0/2.0,1.0/2.0));
    EXPECT_DOUBLE_EQ(e.get_normals()[2][0], -1.0);
    EXPECT_DOUBLE_EQ(e.get_normals()[2][1], 0.0);

}





    