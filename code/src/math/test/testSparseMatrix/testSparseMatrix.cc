/*------------------------------------------------------------------------*\
**  
**  @file:      testSparseMatrix.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit tests for sparse matrix file
**
\*------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include "../../include/sparseMatrix.hh"

#include <vector>


TEST(test_sparse_matrix, Rows_Columns) {
    // Arrange
    MATH::matrixCSR matrix(3,5);

    // Act

    // Assert
    ASSERT_EQ(matrix.get_num_rows(), 3);
    ASSERT_EQ(matrix.get_num_columns(), 5);
}


TEST(test_sparse_matrix, get_value) {
    // Arrange
    MATH::matrixCSR matrix(3,5);
    matrix.set_value(2, 1, 8.0);
    matrix.set_value(0, 1, 2.0);
    matrix.set_value(1, 0, 4.0);
    matrix.set_value(2, 2, 9.0);
    matrix.set_value(1, 1, 5.0);
    matrix.set_value(0, 0, 1.0);
    matrix.set_value(2, 0, 7.0);
    matrix.set_value(0, 2, 3.0);
    matrix.set_value(1, 2, 6.0);

    std::cout << matrix << std::endl;

    // Act

    // Assert
    ASSERT_DOUBLE_EQ(matrix.get_value(0,0), 1.0);
    ASSERT_DOUBLE_EQ(matrix.get_value(0,1), 2.0);
    ASSERT_DOUBLE_EQ(matrix.get_value(0,2), 3.0);

    ASSERT_DOUBLE_EQ(matrix.get_value(1,0), 4.0);
    ASSERT_DOUBLE_EQ(matrix.get_value(1,1), 5.0);
    ASSERT_DOUBLE_EQ(matrix.get_value(1,2), 6.0);

    ASSERT_DOUBLE_EQ(matrix.get_value(2,0), 7.0);
    ASSERT_DOUBLE_EQ(matrix.get_value(2,1), 8.0);
    ASSERT_DOUBLE_EQ(matrix.get_value(2,2), 9.0);
}


TEST(MatrixTest, ScalarMultiplication) {
    // Arrange
    MATH::matrixCSR matrix(3, 3);
    matrix.set_value(0, 0, 1.0);
    matrix.set_value(1, 1, 2.0);
    matrix.set_value(2, 2, -3.0);

    // Act
    auto result = matrix * 2.0;

    // Assert
    ASSERT_DOUBLE_EQ(result.get_value(0, 0), 2.0);
    ASSERT_DOUBLE_EQ(result.get_value(1, 1), 4.0);
    ASSERT_DOUBLE_EQ(result.get_value(2, 2), -6.0);
}

TEST(MatrixTest, VectorMultiplication) {
    // Arrange
    MATH::matrixCSR matrix(3, 4);
    matrix.set_value(0, 0, 1.0);
    matrix.set_value(1, 1, 2.0);
    matrix.set_value(2, 1, -3.0);
    matrix.set_value(2, 2, 4.0);

    // Act
    std::vector<double> v = {2.0, 4.0, -6.0, 1.0};
    auto result = matrix * v;

    // Assert
    ASSERT_DOUBLE_EQ(result[0], 2.0);
    ASSERT_DOUBLE_EQ(result[1], 8.0);
    ASSERT_DOUBLE_EQ(result[2], -36.0);
}