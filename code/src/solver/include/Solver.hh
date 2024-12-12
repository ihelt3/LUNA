/*------------------------------------------------------------------------*\
**  
**  @file:      Solver.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header file for solver base class
**
\*------------------------------------------------------------------------*/

#ifndef _SOLVER_HH_
#define _SOLVER_HH_

#include <memory>
#include "mesh.hh"
#include "sparseMatrix.hh"
#include "fields.hh"
#include "Vector.hh"

// Forward Declarations for circular inclusions
namespace UTILITIES {
    class BoundaryCondition;
}

namespace SOLVER
{

/*------------------------------------------------------------------------*\
**  Class Solver Declaration
\*------------------------------------------------------------------------*/

// Template class for flow solvers
class Solver 
{
protected:
    // Member data
        // smart pointer to mesh
        std::shared_ptr<MESH::mesh> _mesh;
        // Cell Velocity Field
        UTILITIES::field<MATH::Vector> _cellVelocityField;
        // Face Velocity Field
        UTILITIES::field<MATH::Vector> _faceVelocityField;
        // Node Velocity Field
        UTILITIES::field<MATH::Vector> _nodeVelocityField;
        // Cell Pressure Field
        UTILITIES::field<double> _cellPressureField;
        // Face Pressure Field
        UTILITIES::field<double> _facePressureField;
        // Face Mass Flux Field
        UTILITIES::field<double> _faceMassFluxField;
        // Mass flux direction vector [cell idx, face idx]
            // +  =>  into cell
            // -  =>  out of cell
        MATH::matrixCSR _massFluxDirection;
        // Distance between neighboring cells normal to face (len = nfaces)
        std::vector<double> _faceNormalDeltas;
        // Flag if the system has been solved or not
        bool _solved = false;
        // Boundary Conditions Vector (Use smart pointers to allow polymorphism, Solver owns boundary conditions -> boundary conditions use weak pointers)
        std::unordered_map<int, std::shared_ptr<UTILITIES::BoundaryCondition>> _BCs;
        
        // CONSTANTS
            // dynamic viscosity
            double mu = 1.0e-3; //1.0e-3;
            // density
            double rho = 1.0;
            // Velocity Relaxation
            double _urelax = 0.1;
            // Pressure Relaxation
            double _prelax = 0.15;

        // Linear Solver
        // MATH::linear_solver_base<MATH::matrixCSR> _linearSolver;

        // Misc
            // Variables the solver contains
            std::vector<std::string> _variables = {"x","y","z","u","v","p","mdotf"};

    // Member Functions
        // Calculate cell center difference normal to face
        void calculateFaceNormalDeltas();
        
        

public:
    // Constructors 
        // initialize all fields to 0
        Solver(std::shared_ptr<MESH::mesh>);

    // Member data
        // Number of iterations to run
        int iter;
        // Tolerances
            double ptol = 1.0e-3;
            double utol = 1.0e-3;

    // Member Functions
        // Pure virual function for solver
        virtual void solve() {_solved = true;};
        // Calculate cell face pressure gradient
        std::vector<MATH::Vector> computeCellPressureGradient(std::vector<double>);
        // Function to get Pressure gradients across faces
        std::vector<MATH::Vector> computeFacePressureGradient(std::vector<double>, std::vector<double>);
        // Calculate face pressures
        std::vector<double> computeFacePressure(std::vector<double>);
        // Get nodal values using of field
        std::vector<MATH::Vector> computeNodalVector(UTILITIES::field<MATH::Vector>);
        std::vector<double> computeNodalScalar(UTILITIES::field<double>);
        // Set Boundary Conditions
        void setBoundaryCondition(std::shared_ptr<UTILITIES::BoundaryCondition> bc);
        // Check if all boundary conditions are complete
        bool checkBoundaryConditions();
        // Get density
        double get_density() const { return rho; };

    // Get Methods
        // get mesh
        const std::shared_ptr<MESH::mesh> get_mesh() const { return _mesh; };
        // Get boundary conditions
        std::shared_ptr<UTILITIES::BoundaryCondition> get_boundaryCondition(int idx) { return _BCs[idx]; };
        // get face normal deltas
        const std::vector<double> get_faceNormalDeltas() const { return _faceNormalDeltas; };
        // get cell pressure field
        UTILITIES::field<double> get_cellPressureField() const { return _cellPressureField; };
        // get face pressure field
        UTILITIES::field<double> get_facePressureField() const { return _facePressureField; };
        // get velocity field
        UTILITIES::field<MATH::Vector> get_cellVelocityField() const { return _cellVelocityField; };
        // get face velocity field
        UTILITIES::field<MATH::Vector> get_faceVelocityField() const { return _faceVelocityField; };
        // get node velocity field
        UTILITIES::field<MATH::Vector> get_nodeVelocityField() const { return _nodeVelocityField; };
        // get face mass flux field
        UTILITIES::field<double> get_faceMassFluxField() const { return _faceMassFluxField; };
        // get solver variables
        std::vector<std::string> get_variables() const { return _variables; };

    // Set methods
        // set cell pressure field
        const void set_cellPressureField(UTILITIES::field<double> field) { if (!_solved) _cellPressureField = field; else std::cerr << "Solver has already been solved" << std::endl; };
        // set face pressure field
        const void set_facePressureField(UTILITIES::field<double> field) { if (!_solved) _facePressureField = field; else std::cerr << "Solver has already been solved" << std::endl; };


    // Member Data
        
};



}

#endif // _SOLVER_HH_
