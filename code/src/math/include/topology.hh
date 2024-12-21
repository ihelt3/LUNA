/*------------------------------------------------------------------------*\
**  
**  @file:      topology.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header file for topological classes/functions
**
\*------------------------------------------------------------------------*/

#ifndef _TOPOLOGY_HH_
#define _TOPOLOGY_HH_

#include <vector>

#include "Vector.hh"

namespace MATH {

/*------------------------------------------------------------------------*\
**  Jarvis Functions Definitions
\*------------------------------------------------------------------------*/

// Orientation of 3 points
int orientation(Vector p0, Vector p1, Vector q, Vector r);

// jarvis march (or gift wrapping algorithm)  to determine convex hull
std::vector<int> jarvis_march(std::vector<Vector> points);

}

#endif  // _TOPOLOGY_HH_
