/*------------------------------------------------------------------------*\
**  
**  @file:      linearSolvers.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for linear solvers class
**
\*------------------------------------------------------------------------*/

#include <cassert>
#include "linearSolvers.hh"


/*------------------------------------------------------------------------*\
**  Class linear_solver_base Implementation
\*------------------------------------------------------------------------*/
template <class matrix>
void MATH::linear_solver_base<matrix>::check_inputs()
{
    // Check if we have initial guess
    if (!this->x_has_been_set) {
        this->set_guess();
    }

    // make sure we have a square matrix
    assert(this->_A.get_num_rows() == this->_A.get_num_columns() && "Matrix is not square");
    // Ensure vectors are properly size
    assert(this->_x.size() == this->_A.get_num_rows() && this->_b.size() == this->_A.get_num_rows() && "Matrix and Vector sizes don't match");
}

// Explicity Template Instantiation
template class MATH::linear_solver_base<MATH::matrixCSR>;

/*------------------------------------------------------------------------*\
**  Class gauss_seidel Implementation
\*------------------------------------------------------------------------*/

template <class matrix>
MATH::Vector MATH::gauss_seidel<matrix>::solve(unsigned maxIterations, double tolerance)
{
    this->check_inputs();
    
    // refering to inherited members with this->
    // Initialize some values
    Vector x_old;
    double sigma = 0.0;

    // check for trivial case
    if (this->_b.getL2Norm() == 0.0) {
        return MATH::Vector(this->_A.get_num_rows(), 0.0);
    }

    this->_iterations = 0;
    while ( this->_iterations < maxIterations )
    {
        x_old = this->_x;
        for (int i = 0; i < this->_A.get_num_rows(); i++)
        {
            sigma = 0.0;
            
            // Optimized for CSR
            for (int index = this->_A._row_indices[i]; index < this->_A._row_indices[i + 1]; index++) {
                if (this->_A._column_indices[index] < i) {
                    sigma += this->_A._values[index] * this->_x[this->_A._column_indices[index]];
                }
                else if (this->_A._column_indices[index] > i) {
                    sigma += this->_A._values[index] * x_old[this->_A._column_indices[index]];
                }
            }

            
            // Check for zero diagonal and solve
            double diag = this->_A.get_value(i, i);
            if (std::abs(diag) < 1e-12) {  // Handle zero or near-zero diagonals
                throw std::runtime_error("Zero or near-zero diagonal element in matrix");
            }
            this->_x[i] = (this->_b[i] - sigma) / diag;
        }

        // Calculate residual norm
        this->_resid = (this->_A * this->_x - this->_b).getL2Norm();
        if (this->_resid < tolerance)
        {
            break;
        }

        // Update Iterations 
        this->_iterations++;
    }
    
    // TODO
    return this->_x;
}

// Explicity Template Instantiation
template class MATH::gauss_seidel<MATH::matrixCSR>;


/*------------------------------------------------------------------------*\
**  Class conjugate_gradient Implementation
\*------------------------------------------------------------------------*/

template <class matrix>
MATH::Vector MATH::conjugate_gradient<matrix>::solve(unsigned maxIterations, double tolerance)
{
    this->_iterations = 0;
    this->check_inputs();

    // check for trivial case
    if (this->_b.getL2Norm() == 0.0) {
        return MATH::Vector(this->_A.get_num_rows(), 0.0);
    }

    // Initializing some vectors
    Vector q(this->_b.size());
    Vector rold(this->_b.size());
    double alpha;
    double beta;

    // Get initial residual 
    Vector r = this->_b - this->_A * this->_x;
    rold = r;
    Vector p = r;

    // See MATH 6644 Notes
    while ( this->_iterations < maxIterations)
    {
        q = this->_A * p;
        alpha = (r.transpose() * r) / (p.transpose() * q);
        this->_x = this->_x + p*alpha;
        r = rold - q*alpha;
        beta = (r.transpose() * r) / (rold.transpose() * rold);
        p = r + p*beta;
        rold = r;
        this->_resid = r.getL2Norm();
        if (this->_resid < tolerance)
        {
            return this->_x;
        }
        this->_iterations++;
    }
    return this->_x;
}

// Explicity Template Instantiation
template class MATH::conjugate_gradient<MATH::matrixCSR>;