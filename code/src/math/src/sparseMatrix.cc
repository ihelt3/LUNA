/*------------------------------------------------------------------------*\
**  
**  @file:      sparseMatrix.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for sparse matrix
**
\*------------------------------------------------------------------------*/

#include "sparseMatrix.hh"

#include <cassert>
#include <vector>


/*------------------------------------------------------------------------*\
**  Class sparseMatrix Implementation
\*------------------------------------------------------------------------*/
MATH::sparseMatrixBase::sparseMatrixBase(int num_rows, int num_columns) 
:
    _num_rows(num_rows),
    _num_columns(num_columns)
{}

std::ostream& MATH::operator<<(std::ostream& os, const MATH::sparseMatrixBase& matrix) {
    for (int i = 0; i < matrix._num_rows; i++) {
        for (int j = 0; j < matrix._num_columns; j++) {
            os << matrix.get_value(i, j) << " ";
        }
        os << "\n";
    }
    return os;
}


/*------------------------------------------------------------------------*\
**  Class matrixCSR Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructors * * * * * * * * * * * * * * * //
// MATH::matrixCSR::matrixCSR(int num_rows, int num_columns)
// {
    
// }


// * * * * * * * * * * * * * *  get_value * * * * * * * * * * * * * * * //
double MATH::matrixCSR::get_value(int row, int col) const {
    assert(row < _num_rows && col < _num_columns);

    // Gets total number of entries in row
    for (int i = _row_indices[row]; i < _row_indices[row+1]; i++) {
        // Checks columns given in row index to see if they coicide with given column
        if (_column_indices[i] == col) {
            return _values[i];
        }
    }
    // returns 0 if not match (sparse matrix)
    return 0.0;
}


// * * * * * * * * * * * * * *  set_value * * * * * * * * * * * * * * * //
// NOTE: Does not allow 0 to be stored
void MATH::matrixCSR::set_value(int row, int col, double value) {
    assert(row < _num_rows && col < _num_columns);
    
    // First check if row/column already has NZ entry
    bool rowColumnHasEntry = false;
    unsigned index = 0;
    for (unsigned i = _row_indices[row]; i < _row_indices[row + 1]; i++) {
        if (_column_indices[i] == col) {
            index = i;
            rowColumnHasEntry = true;
        }
    }

    // make sure values exists
    if (value != 0.0) {
        // if row already has data in it, 
        if (rowColumnHasEntry) {
            _values[index] = value;
        } 
        else {
            _values.insert(_values.begin() + _row_indices[row + 1], value);
            _column_indices.insert(_column_indices.begin() + _row_indices[row + 1], col);

            for (unsigned i = row + 1; i <= _num_rows; i++) {
                _row_indices[i]++;
            }
        }
    
    }
    else if (rowColumnHasEntry) {
        // Decrease number of non-zero entries in row
        for (unsigned i = row + 1; i <= _num_rows; i++) {
            _row_indices[i]--;
        }

        // Remove column entry
        _values.erase(_values.begin() + index);
        _column_indices.erase(_column_indices.begin() + index);
        
    }
}


// * * * * * * * * * * * * * *  scalar multiplication with * operator * * * * * * * * * * * * * * * //
MATH::matrixCSR MATH::matrixCSR::operator*(const double &scaleFactor) const {
    MATH::matrixCSR result(*this);
    for (unsigned i = 0; i < result._values.size(); ++i) {
        result._values[i] *= scaleFactor;
    }
    return result;
}


// * * * * * * * * * * * * * *  vector multiplication with * operator * * * * * * * * * * * * * * * //
// Note: const because it does not modify the given vector (rhs)
MATH::Vector MATH::matrixCSR::operator*(const Vector& rhs) const {
    assert(rhs.size() == _num_columns && "Vector size does not match the number of columns in the matrix.");

    Vector result(_num_rows);

    for (unsigned row = 0; row < _num_rows; ++row) {
        for (unsigned columnIndex = _row_indices[row]; columnIndex < _row_indices[row + 1]; ++columnIndex) {
            auto column = _column_indices[columnIndex];
            result[row] += _values[columnIndex] * rhs[column];
        }
    }

    return result;
}