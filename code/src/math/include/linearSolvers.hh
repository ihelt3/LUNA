/*------------------------------------------------------------------------*\
**  
**  @file:      linearSolvers.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header for linear solvers
**
\*------------------------------------------------------------------------*/

#ifndef _LINEARSOLVERS_HH_
#define _LINEARSOLVERS_HH_

#include <vector>
#include "sparseMatrix.hh"
#include "Vector.hh"

namespace MATH {

/*------------------------------------------------------------------------*\
**  Class linear_solver_base Declaration
\*------------------------------------------------------------------------*/

// Template for matrix class
template <class matrix>
class linear_solver_base
{

public:
    // Constructor
        linear_solver_base() : _A(0,0), _b(0), _x(0) {}; // initialize A as empty matrix

    // Member Functions
        void check_inputs();
        // Set all the relevent member data
        void set_matrix(matrix A) { _A = A; };
        void set_rhs(Vector b) { _b = b; };
        void set_rhs(std::vector<double> b) { _b=Vector(b); };
        // Set guess of solution (required)
        void set_guess(std::vector<double> x) { _x = Vector(x); x_has_been_set = true; };
        void set_guess(Vector x) { _x = x; x_has_been_set = true; };
        void set_guess() { _x = _b; x_has_been_set = true; };
        // Solve the linear system
        virtual Vector solve(unsigned maxIterations, double tolerance) = 0; // pure virtual function

    // get member functions
        matrix get_matrix() const { return _A; };
        Vector get_rhs() const { return _b; };
        double get_residual() const { return _resid; };
        int get_iterations() const { return this->_iterations; };

protected:
    // Member Data
        matrix _A;
        Vector _b;
        Vector _x;
        double _resid = 0.0;
        int _iterations = 0;
        bool x_has_been_set = false;
};


/*------------------------------------------------------------------------*\
**  Class gauss_seidel Declaration
\*------------------------------------------------------------------------*/

template <class matrix>
class gauss_seidel 
: 
    public linear_solver_base<matrix>
{

public:
    // Constructor
    gauss_seidel() : linear_solver_base<matrix>() {};

    // Member Functions
        Vector solve(unsigned maxIterations, double tolerance) override;

    // Member Data
};


/*------------------------------------------------------------------------*\
**  Class conjugate_gradient Declaration
\*------------------------------------------------------------------------*/

template <class matrix>
class conjugate_gradient
: 
    public linear_solver_base<matrix>
{

public:
    // Constructor
    conjugate_gradient() : linear_solver_base<matrix>() {};

    // Member Functions
        Vector solve(unsigned maxIterations, double tolerance) override;

    // Member Data
};


}

#endif // _LINEARSOLVERS_HH_