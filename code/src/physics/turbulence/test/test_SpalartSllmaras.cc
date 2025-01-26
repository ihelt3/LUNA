/*------------------------------------------------------------------------*\
**  
**  @file:      test_SpalartAllmaras.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit test for Spalart-Allmaras turbulence model
**
\*------------------------------------------------------------------------*/

#include "gtest/gtest.h"

#include "SpalartAllmaras.hh"
#include "Solver.hh"
#include "mesh.hh"
#include "read_su2.hh"
#include "Vector.hh"


class test_outlet : public ::testing::Test 
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
    test_outlet() {
        complexInitialization();
    }

};


// NOTHING TESTED HERE YET

