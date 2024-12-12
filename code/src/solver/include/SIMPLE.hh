/*------------------------------------------------------------------------*\
**  
**  @file:      SIMPLE.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header file for SIMPLE class
**
\*------------------------------------------------------------------------*/

#ifndef _SIMPLE_HH_
#define _SIMPLE_HH_

#include <memory>
#include <algorithm>

#include "Solver.hh"
#include "BoundaryConditions.hh"
#include "fields.hh"
#include "linearSolvers.hh"
#include "MeshEntities.hh"

namespace SOLVER
{

/*------------------------------------------------------------------------*\
**  Class SIMPLE Declaration
\*------------------------------------------------------------------------*/
class SIMPLE
:
    public Solver
{

public:
    // Constructors
        // Constructor with just mesh
        SIMPLE(std::shared_ptr<MESH::mesh>);

    // Member Functions
        // Solve function which acts as a wrapper
        void solve() override;
        // Solve the Momentum equation
        void solveMomentumSystem();
        // Solve for the Pressure Correction
        void SolvePressureCorrection();
        // Correct the cell velocity
        void correctCellVelocity();
        // correct the face mass flux (also corrects the face velocity)
        void correctFaceMassFlux();
        // calculate the face mass flux
        void computeFaceMassFlux();

        // POSSIBLY SOME FAT TO CUT
        // Function to get face velocities 
        void computeFaceVelocities();
        

    // Member Data


private:
    // Member data
        // Momentum system (A)
        MATH::matrixCSR _momentumSystemA;
        // Momentum system (b)
        MATH::Vector _momentumSystemb_x;
        MATH::Vector _momentumSystemb_y;
        MATH::Vector _momentumSystemb_z;
        // Pressure Correction
        MATH::Vector _pressureCorrection;
        
    // Member Functions
        // Update momentum system of equations
        void updateMomentumMatrix();
        // Update Moment RHS
        void updateMomentumRHS();
        // Check for convergence
        bool checkConvergence();

};

}


#endif // _SIMPLE_HH_