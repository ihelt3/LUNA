/*------------------------------------------------------------------------*\
**  
**  @file:      main.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     main file for testing library execution
**
\*------------------------------------------------------------------------*/

#include <vector>
#include ".\mainInclude.hh"

// Forward declarations
// template <typename meshtypeCellTest>
// void test_cells(meshtypeCellTest &mesh);
// template <typename meshtypeNodeTest>
// void test_nodes(meshtypeNodeTest &mesh);
// template <typename meshtypeVolumeTest>
// void test_cellVolume(meshtypeVolumeTest &mesh);
// template <typename connectivityTest>
// void test_connectivity(connectivityTest &mesh);


/*------------------------------------------------------------------------*\
** Main
\*------------------------------------------------------------------------*/
int main() {

    // const int nx=10, ny=10, nz=10;
    // std::vector<std::vector<int>> grid[nx][ny][nz];

    // auto testMeshFile = std::filesystem::path("../solver/test/SIMPLE/meshes/testMesh.su2");
    auto testMeshFile = std::filesystem::path("./test/su2/meshes/square_spacing5e-3.su2");
    // auto testMeshFile = std::filesystem::path("../../Examples/SquareCavityFlow/squareCavity_unstruct.su2");
    MESH::read_su2 test_read_su2(testMeshFile);

    MESH::mesh mesh = test_read_su2.get_mesh();

    // MESH::uniform_partition<MESH::read_su2> partition(test_read_su2);

    /*
    // Run tests
    std::cout << "Beginning tests..." << std::endl;
    std::vector<std::vector<int>> cellConnectivity = test_read_su2.get_cellConnectivity();
    std::cout << "cell connectivity: ";
    for (int i=0 ; i<cellConnectivity[0].size() ; i++) {
        std::cout << cellConnectivity[0][i] << ", ";
    }
    std::cout << std::endl;
    test_cells(test_read_su2);
    test_nodes(test_read_su2);
    test_cellVolume(test_read_su2);
    test_connectivity(test_read_su2);
    */

    // std::cout << "Cell centroids: " << std::endl;
    // std::vector<MESH::cell> cells = test_read_su2.get_cells();
    // for (int i=0 ; i<cells.size() ; i++) {
    //     std::cout << "Cell ID: " << i;
    //     for (int j=0 ; j<cells[i].get_cellElement().get_centroid().get_coordinates().size() ; j++) {
    //         std::cout << "   " << cells[i].get_cellElement().get_centroid().get_coordinates()[j];
    //     }
    //     std::cout << std::endl;
    // }

    

}


/*------------------------------------------------------------------------*\
** Tests
\*------------------------------------------------------------------------*/


// /*---------------------------------- Test Cell Node Reading -----------------------------------*/
// // Test read in cells
// template <typename meshtypeCellTest>
// void test_cells(meshtypeCellTest &mesh) {
// std::vector<MESH::cell> cells = mesh.get_cells();

// std::vector<int> nodes;

// nodes = cells[0].get_cellElement().get_nodeIDs();
// assert(nodes[0] == 0 && "Unexpected node value");
// assert(nodes[1] == 1 && "Unexpected node value");
// assert(nodes[2] == 3 && "Unexpected node value");
// nodes = cells[1].get_cellElement().get_nodeIDs();
// assert(nodes[0] == 1 && "Unexpected node value");
// assert(nodes[1] == 4 && "Unexpected node value");
// assert(nodes[2] == 3 && "Unexpected node value");
// nodes = cells[2].get_cellElement().get_nodeIDs();
// assert(nodes[0] == 1 && "Unexpected node value");
// assert(nodes[1] == 2 && "Unexpected node value");
// assert(nodes[2] == 4 && "Unexpected node value");
// nodes = cells[3].get_cellElement().get_nodeIDs();
// assert(nodes[0] == 2 && "Unexpected node value");
// assert(nodes[1] == 5 && "Unexpected node value");
// assert(nodes[2] == 4 && "Unexpected node value");
// nodes = cells[4].get_cellElement().get_nodeIDs();
// assert(nodes[0] == 3 && "Unexpected node value");
// assert(nodes[1] == 4 && "Unexpected node value");
// assert(nodes[2] == 6 && "Unexpected node value");
// nodes = cells[5].get_cellElement().get_nodeIDs();
// assert(nodes[0] == 4 && "Unexpected node value");
// assert(nodes[1] == 7 && "Unexpected node value");
// assert(nodes[2] == 6 && "Unexpected node value");
// nodes = cells[6].get_cellElement().get_nodeIDs();
// assert(nodes[0] == 4 && "Unexpected node value");
// assert(nodes[1] == 5 && "Unexpected node value");
// assert(nodes[2] == 7 && "Unexpected node value");
// nodes = cells[7].get_cellElement().get_nodeIDs();
// assert(nodes[0] == 5 && "Unexpected node value");
// assert(nodes[1] == 8 && "Unexpected node value");
// assert(nodes[2] == 7 && "Unexpected node value");

// std::cout << "  cell test passed!" << std::endl;
// };


// /*---------------------------------- Test Node Coordinate Reading -----------------------------------*/
// // Test read in cells
// template <typename meshtypeNodeTest>
// void test_nodes(meshtypeNodeTest &mesh) {
// std::vector<MESH::node> nodes = mesh.get_nodes();

// std::vector<double> coords;

// coords = nodes[0].get_coordinates();
// assert(coords[0] == 0.0 && "Unexpected node value");
// assert(coords[1] == 0.0 && "Unexpected node value");
// coords = nodes[1].get_coordinates();
// assert(coords[0] == 0.5 && "Unexpected node value");
// assert(coords[1] == 0.0 && "Unexpected node value");
// coords = nodes[2].get_coordinates();
// assert(coords[0] == 1.0 && "Unexpected node value");
// assert(coords[1] == 0.0 && "Unexpected node value");
// coords = nodes[3].get_coordinates();
// assert(coords[0] == 0.0 && "Unexpected node value");
// assert(coords[1] == 0.5 && "Unexpected node value");
// coords = nodes[4].get_coordinates();
// assert(coords[0] == 0.5 && "Unexpected node value");
// assert(coords[1] == 0.5 && "Unexpected node value");
// coords = nodes[5].get_coordinates();
// assert(coords[0] == 1.0 && "Unexpected node value");
// assert(coords[1] == 0.5 && "Unexpected node value");
// coords = nodes[6].get_coordinates();
// assert(coords[0] == 0.0 && "Unexpected node value");
// assert(coords[1] == 1.0 && "Unexpected node value");
// coords = nodes[7].get_coordinates();
// assert(coords[0] == 0.5 && "Unexpected node value");
// assert(coords[1] == 1.0 && "Unexpected node value");
// coords = nodes[8].get_coordinates();
// assert(coords[0] == 1.0 && "Unexpected node value");
// assert(coords[1] == 1.0 && "Unexpected node value");

// std::cout << "  node test passed!" << std::endl;
// }

// /*---------------------------------- Test Cell Volume Calculations -----------------------------------*/
// template <typename meshtypeVolumeTest>
// void test_cellVolume(meshtypeVolumeTest &mesh) {

// std::vector<MESH::cell> cells = mesh.get_cells();
// // All cells are the same tri, so volume should be the same
// for (int i=0 ; i<cells.size() ; i++) {
//     assert(std::fabs(cells[i].get_cellElement().get_volume() - pow(0.5,3.0)) < 0.0001 && "Unexpected cell volume");
// }

// std::cout << "  cell Volume test passed!" << std::endl;
// }


// /*---------------------------------- Test Cell Connectivity -----------------------------------*/
// template <typename connectivityTest>
// void test_connectivity(connectivityTest &mesh) {

//     // Check cell Connectivity
//     std::vector<std::vector<int>> cellcon = mesh.get_cellConnectivity();
//     //
//     assert(cellcon[0][0] == -1 && "Unexpected cell connectivity");
//     assert(cellcon[0][1] == 1 && "Unexpected cell connectivity");
//     assert(cellcon[0][2] == -1 && "Unexpected cell connectivity");
//     //
//     assert(cellcon[1][0] == 2 && "Unexpected cell connectivity");
//     assert(cellcon[1][1] == 4 && "Unexpected cell connectivity");
//     assert(cellcon[1][2] == 0 && "Unexpected cell connectivity");
//     //
//     assert(cellcon[2][0] == -1 && "Unexpected cell connectivity");
//     assert(cellcon[2][1] == 3 && "Unexpected cell connectivity");
//     assert(cellcon[2][2] == 1 && "Unexpected cell connectivity");
//     //
//     assert(cellcon[3][0] == -1 && "Unexpected cell connectivity");
//     assert(cellcon[3][1] == 6 && "Unexpected cell connectivity");
//     assert(cellcon[3][2] == 2 && "Unexpected cell connectivity");
//     //
//     assert(cellcon[4][0] == 1 && "Unexpected cell connectivity");
//     assert(cellcon[4][1] == 5 && "Unexpected cell connectivity");
//     assert(cellcon[4][2] == -1 && "Unexpected cell connectivity");
//     //
//     assert(cellcon[5][0] == 6 && "Unexpected cell connectivity");
//     assert(cellcon[5][1] == -1 && "Unexpected cell connectivity");
//     assert(cellcon[5][2] == 4 && "Unexpected cell connectivity");
//     //
//     assert(cellcon[6][0] == 3 && "Unexpected cell connectivity");
//     assert(cellcon[6][1] == 7 && "Unexpected cell connectivity");
//     assert(cellcon[6][2] == 5 && "Unexpected cell connectivity");
//     //
//     assert(cellcon[7][0] == -1 && "Unexpected cell connectivity");
//     assert(cellcon[7][1] == -1 && "Unexpected cell connectivity");
//     assert(cellcon[7][2] == 6 && "Unexpected cell connectivity");

//     std::vector<std::vector<int>> bccon = mesh.get_BCConnectivity();


//     std::cout << "  cell Connectivity test passed!" << std::endl;
// }