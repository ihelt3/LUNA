/*------------------------------------------------------------------------*\
**  
**  @file:      topology.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     implementation file for topological classes/functions
**
\*------------------------------------------------------------------------*/

#define _USE_MATH_DEFINES

#include <cassert>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>

#include "topology.hh"


/*------------------------------------------------------------------------*\
**  Jarvis March Algorithm Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * orientation * * * * * * * * * * * * * * * //
                    //  origin          test point 1      test point 2
int MATH::orientation(MATH::Vector p0, MATH::Vector p1, MATH::Vector q, MATH::Vector r)
{
    /*
    int val = (q[1] - p[1]) * (r[0] - q[0]) -
              (q[0] - p[0]) * (r[1] - q[1]);
    
    // std::cout << "     val: " << val << std::endl;
    if (val == 0) return 0;  // collinear OR vertical/horizontal points
    return (val > 0)? 1: 2; // clock or counterclock wise, respectively
    */ 

    double eps = 1e-10;
    // ensure new point is not coincident with p1
    if (std::abs(r[0] - p1[0]) < eps && std::abs(r[1] - p1[1]) < eps) return 0;

    // Reference angle
    double theta_ref = M_PI/2.0 - std::atan2(p0[1] - p1[1], p0[0] - p1[0]);
    if (theta_ref < 0.0) theta_ref += 2.0 * M_PI;

    // Let's use 4 quadrant arc-tangent instead
    double thetaq = (M_PI/2.0 - std::atan2(q[1] - p1[1], q[0] - p1[0])) - theta_ref;
    double thetar = (M_PI/2.0 - std::atan2(r[1] - p1[1], r[0] - p1[0])) - theta_ref;

    // Convert back to unit circle, so angles are clockwise relative to positive y-axis
    if (thetaq < 0.0) thetaq += 2.0 * M_PI;
    if (thetar < 0.0) thetar += 2.0 * M_PI;

    // Only if the other point is in the top left quadrant to want a vertical point
    if (thetaq == 0.0 && thetar >= (3.0/2.0)*M_PI) thetaq = 2.0 * M_PI;
    if (thetar == 0.0 && thetaq >= (3.0/2.0)*M_PI) thetar = 2.0 * M_PI;

    // Check angles
    if (thetaq - thetar > eps) return 1;        // q more clockwise than r
    else if (thetaq - thetar < eps) return 2;   // r more clockwise than q
    else return 0;                              // points are colinear
}

// Jarvis March Algorithm (modified for LUNA purposes): returns indices of points on convex hull
std::vector<int> MATH::jarvis_march(std::vector<MATH::Vector> points) {
    assert(points.size() >= 3 && "Jarvis march requires at least 3 points");

    for (const auto& point : points) {
        assert(point.size() == 2 && "Jarvis march requires Vectors must be 2D");
    }

    int leftmost = 0;
    for (int i = 1; i < points.size(); i++) {
        if (points[i][0] < points[leftmost][0]) {
            leftmost = i;
        }
    }

    std::vector<int> hull;
    int p = leftmost;
    int q;
    int orient = 0;

    do {
        hull.push_back(p);
        q = (p + 1) % points.size();

        for (int i = 0; i < points.size(); i++) {
            // Check point orientation
            if (hull.size() == 1) {
                // orientation relative to the vertical
                orient = orientation(points[p]+MATH::Vector(std::vector<double>{0.0,1.0}), points[p], points[q], points[i]);
            }
            else {
                orient = orientation(points[hull.rbegin()[1]], points[p], points[q], points[i]);
            }
            // Evauluate result of orientation
            if (orient == 2) {
                q = i;
            }
        }

        p = q;

    } while (p != leftmost);

    return hull;
}


