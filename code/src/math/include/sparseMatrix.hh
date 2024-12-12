/*------------------------------------------------------------------------*\
**  
**  @file:      sparseMatrix.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     header for sparse matrix class
**
\*------------------------------------------------------------------------*/

#ifndef _SPARSEMATRIX_HH_
#define _SPARSEMATRIX_HH_

#include <vector>
#include "Vector.hh"

namespace MATH {

class sparseMatrixBase
{
public:
    // Constructor
        sparseMatrixBase(int num_rows, int num_columns);

    // Get member functions
        int get_num_rows() const { return _num_rows; };
        int get_num_columns() const { return _num_columns; };

    // pure Virtual get and set member functions
        virtual double get_value(int i, int j) const = 0;
        virtual void set_value(int i, int j, double value) = 0;

    // pure virtual operator
        virtual Vector operator*(const Vector& rhs) const = 0;

    // Operator to print matrix
        friend std::ostream& operator<<(std::ostream& os, const sparseMatrixBase& matrix);

protected:
    int _num_rows;
    int _num_columns;
};

/*------------------------------------------------------------------------*\
**  Class matrixCSR Declaration
\*------------------------------------------------------------------------*/

class matrixCSR
:
    public sparseMatrixBase
{
public:
    // friend declarations
    template <class T>
    friend class gauss_seidel;

public:
    // Constructor
        matrixCSR(int num_rows, int num_columns)
            : sparseMatrixBase(num_rows, num_columns) 
        {_row_indices.resize(_num_rows+1,0.0);};

    // Member Functions
        double get_value(int i, int j) const override;
        void set_value(int i, int j, double value) override;

    // Overloaded Operators
        matrixCSR operator*(const double &scaleFactor) const;
        Vector operator*(const Vector& rhs) const override;

private:
    // Member Data
    std::vector<double>_values;
    std::vector<int> _row_indices;
    std::vector<int> _column_indices;
};

}

#endif // _SPARSEMATRIX_HH_