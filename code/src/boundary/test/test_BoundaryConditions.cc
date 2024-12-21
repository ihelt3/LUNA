/*------------------------------------------------------------------------*\
**  
**  @file:      test_BoundaryConditions.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit tests for Boundary Condiitons classes
**
\*------------------------------------------------------------------------*/

#include <vector>
#include <filesystem>
#include <memory>

#include "gtest/gtest.h"
#include "Solver.hh"
#include "BoundaryConditions.hh"
#include "mesh.hh"
#include "read_su2.hh"
#include "Vector.hh"

class test_BCs : public ::testing::Test 
{
protected:
    // Member data
    std::shared_ptr<MESH::mesh> su2Mesh;
    std::shared_ptr<SOLVER::Solver> solver;

    // Initializer
    void complexInitialization() {
        // Simple mesh to test with
        std::filesystem::path meshFile("test/Common/rectangle.su2");

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


