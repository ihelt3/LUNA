/*------------------------------------------------------------------------*\
**  
**  @file:      partitioning.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header file for partitioning nodes for connectivity checks
**
\*------------------------------------------------------------------------*/

#ifndef _PARTITIONING_HH_
#define _PARTITIONING_HH_

#include <vector>
#include "read_base.hh"
#include "MeshEntities.hh"

namespace MESH
{

/*------------------------------------------------------------------------*\
**  Class uniform_partition Declaration
\*------------------------------------------------------------------------*/

template <class meshtype = read_base>
class uniform_partition 
{
public:
    // Construction
    uniform_partition(meshtype mesh_in) : mesh(mesh_in) {};

    // Member functions
    void initialize_bins();

    // Member data
    meshtype mesh;
    typedef std::vector<std::vector<std::vector<std::vector<int>>>> bin_type;
    std::vector<std::vector<int>> bin_lookup;
    std::vector<int>* bin_grid;
    std::vector<std::vector<double>> minmax{{1.0e32,-1.0e32},{1.0e32,-1.0e32},{1.0e32,-1.0e32}};
};

template <class meshtype>
void uniform_partition<meshtype>::initialize_bins() 
{
    std::vector<node> nodes = mesh.get_nodes();

    // Determine max extents in each direction
    int d = mesh.get_dimension();
    minmax.resize(d);
    std::vector<double> coord;
    for (int i=0 ; i<nodes.size() ; i++)
    {
        for (int j=0 ; j<d ; j++) 
        {
            coord = nodes[i].get_coordinates();
            if (coord[j] < minmax[j][0]) {
                minmax[j][0] = coord[j];
            }
            else if (coord[j] > minmax[j][1]) {
                minmax[j][1] = coord[j];
            }
        }
    }

    // Resize elements based on mesh
    // int nx=10, ny=10, nz=10;
    // bin_grid = new int[nx][ny][nz];
    // bin_lookup.resize(nodes.size());
    
};


}

#endif // _PARTITIONING_HH_