/*------------------------------------------------------------------------*\
**  
**  @file:      testLinearSolvers.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit tests for linear solvers
**
\*------------------------------------------------------------------------*/


#include <gtest/gtest.h>

#include "sparseMatrix.hh"
#include "linearSolvers.hh"

#include <vector>

class solverTest 
: 
public 
    ::testing::Test 
{
public:
    // Constructor
    solverTest() : matrix(3,3) {
        matrix.set_value(0,0,1.0);
        matrix.set_value(0,1,1.0);
        matrix.set_value(0,2,1.0);
        matrix.set_value(1,0,0.0);
        matrix.set_value(1,1,2.0);
        matrix.set_value(1,2,3.0);
        matrix.set_value(2,0,0.0);
        matrix.set_value(2,1,8.0);
        matrix.set_value(2,2,6.0);
    }
protected:
    double tol = 1.0e-5;
    int iterations = 100;
    MATH::matrixCSR matrix;
    

    std::vector<double> rhs = {1.0,2.0,3.0};
    std::vector<double> x = { 5.0/12.0, -1.0/4.0, 5.0/6.0 };
};

TEST(test_base_solver, testSetMatrix) {
    // Arrange
    MATH::matrixCSR matrix(3,5);
    matrix.set_value(1,1,1.0);
    MATH::conjugate_gradient<MATH::matrixCSR> solver;
    solver.set_matrix(matrix);

    // Act
    auto getMat = solver.get_matrix();

    // Assert
    ASSERT_EQ(matrix.get_num_rows(), 3);
    ASSERT_EQ(matrix.get_num_columns(), 5);
    ASSERT_DOUBLE_EQ(getMat.get_value(1,1), 1.0);
}

// * * * * * * * * * * * * * * * * * * Test Gauss Seidel Method * * * * * * * * * * * * * * * * * * //
TEST_F(solverTest, testGaussSeidel) {
    matrix.set_value(0,0 , 2.0);
    matrix.set_value(0,1 ,-1.0);
    matrix.set_value(0,2 , 0.0);
    matrix.set_value(1,0 ,-1.0);
    matrix.set_value(1,1 , 2.0);
    matrix.set_value(1,2 ,-1.0);
    matrix.set_value(2,0 , 0.0);
    matrix.set_value(2,1 ,-1.0);
    matrix.set_value(2,2 , 2.0);

    // Arrange
    MATH::gauss_seidel<MATH::matrixCSR> solver;
    solver.set_matrix(matrix);
    solver.set_rhs(std::vector<double>{1.0,0.0,1.0});

    // Act
    auto sol = solver.solve(iterations,tol);

    // Assert
    ASSERT_NEAR(sol[0], 1.0 , tol);
    ASSERT_NEAR(sol[1], 1.0 , tol);
    ASSERT_NEAR(sol[2], 1.0 , tol);
}

// * * * * * * * * * * * * * * * * * * Test Conjugate Gradient Method * * * * * * * * * * * * * * * * * * //
TEST_F(solverTest, testConjugateGradient) {
    // Arrange
    MATH::conjugate_gradient<MATH::matrixCSR> solver;
    solver.set_matrix(matrix);
    solver.set_rhs(rhs);

    // Act
    auto sol = solver.solve(iterations,tol);

    // Assert
    ASSERT_NEAR(sol[0], x[0] , tol);
    ASSERT_NEAR(sol[1], x[1] , tol);
    ASSERT_NEAR(sol[2], x[2] , tol);
}
