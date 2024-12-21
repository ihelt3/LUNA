/*------------------------------------------------------------------------*\
**  
**  @file:      Vector.cc
**
**  @author:    Tom-Robin Teschner (cfd.university)
**
**  @brief:     implementation file for Vector class
**
\*------------------------------------------------------------------------*/

#include "Vector.hh"

namespace MATH {

Vector::Vector() {
  _vector.resize(0);
}

/**
 * @brief Constructor for Vector class
 * @details Creates a new Vector of given size, all elements are set to 0.0.
 * @param size The size of the new Vector
 */

Vector::Vector(const unsigned &size) {
  _vector.resize(size, 0.0);
}

// Construct vector class from std::vector
Vector::Vector(const std::vector<double> &vector) : _vector(vector) {}

/**
 * @brief Returns the size of the Vector
 * @return The size of the Vector
*/
unsigned Vector::size() const {
  return _vector.size();
}

// Construct vector class from size and value
Vector::Vector(const unsigned &size, double value) {
  _vector.resize(size,value);
}

/**
 * @brief Returns a transposed copy of the Vector
 * @details The transpose is a deep copy of the Vector, meaning that the new Vector is independent of the original one.
 * @return The transposed Vector
 */
Vector Vector::transpose() {
  Vector transposedVector(_vector.size());
  transposedVector._vector = _vector;
  transposedVector._isRowVector = _isRowVector == true ? false : true;
  return transposedVector;
}

/**
 * @brief Returns the L2Norm of the Vector
 *
 * @details The L2Norm of a vector is the square root of the sum of the squares of all its elements.
 *
 * @return The L2Norm of the Vector
 */
double Vector::getL2Norm() const {
  double L2Norm = 0.0;
  for (const auto &entry : _vector)
    L2Norm += std::pow(entry, 2);
  L2Norm = std::sqrt(L2Norm);
  return L2Norm;
}

/**
 * @brief Element-wise addition of two Vectors
 *
 * @details The result is a Vector with the same size as the operands, where each element
 * is the sum of the corresponding elements of the two operands.
 *
 * @param other Vector to add to this Vector
 *
 * @return The result of the element-wise addition
 */
Vector Vector::operator+(const Vector &other) const {
  assert(_vector.size() == other._vector.size() && "vectors must have the same dimension");
  Vector resultVector(_vector.size());
  resultVector._vector.resize(_vector.size());
  for (unsigned i = 0; i < _vector.size(); ++i)
    resultVector._vector[i] = _vector[i] + other._vector[i];
  return resultVector;
}

/**
 * @brief Element-wise subtraction of two Vectors
 *
 * @details The result is a Vector with the same size as the operands, where each element
 * is the difference of the corresponding elements of the two operands.
 *
 * @param other Vector to subtract from this Vector
 *
 * @return The result of the element-wise subtraction
 */
Vector Vector::operator-(const Vector &other) const {
  assert(_vector.size() == other._vector.size() && "vectors must have the same dimension");
  Vector resultVector(_vector.size());
  resultVector._vector.resize(_vector.size());
  for (unsigned i = 0; i < _vector.size(); ++i)
    resultVector._vector[i] = _vector[i] - other._vector[i];
  return resultVector;
}

double &Vector::operator[](unsigned index) {
  return _vector[index];
}

const double &Vector::operator[](unsigned index) const {
  return _vector[index];
}


/**
 * @brief Computes the dot product between two Vectors
 *
 * @details The first Vector must be a row vector, and the second Vector must be a column vector.
 *
 * @param other The column Vector to compute the dot product with.
 *
 * @return The computed dot product.
 */
double Vector::operator*(const Vector &other) const {
  // assert(_isRowVector && !other._isRowVector && "first vector must be a row vector, second must be a column vector");
  double sum = 0.0;
  for (unsigned i = 0; i < _vector.size(); ++i)
    sum += _vector[i] * other._vector[i];
  return sum;
}

/**
 * @brief Element-wise multiplication of a Vector with a scalar
 *
 * @details The result is a Vector with the same size as the operand, where each element
 * is the product of the corresponding element of the operand with the given scalar.
 *
 * @param scaleFactor The scalar to multiply the Vector with.
 *
 * @return The result of the element-wise multiplication.
 */
Vector operator*(const double &scaleFactor, Vector vector) {
  for (unsigned index = 0; index < vector.size(); ++index)
  {
    vector[index] *= scaleFactor;
  }
  return vector;
}

Vector operator*(Vector vector, const double &scaleFactor) {
  for (unsigned index = 0; index < vector.size(); ++index)
  {
    vector[index] *= scaleFactor;
  }
  return vector;
}


// Whereas this division does not modify the current vector
// USAGE: vector2 = vector1 / divisor
Vector operator/(Vector vector, const double &divisor) {
  for (unsigned index = 0; index < vector.size(); ++index)
    vector[index] /= divisor;
  return vector;
}

std::ostream &operator<<(std::ostream &out, const Vector &vector) {
  out << "( ";
  for (unsigned index = 0; index < vector.size(); ++index)
    out << vector[index] << " ";
  out << ")";
  return out;
}

bool Vector::operator==(const Vector &other) const {
  if (_vector.size() != other._vector.size())
    return false;

  for (unsigned index = 0; index < _vector.size(); ++index)
    if (_vector[index] != other._vector[index])
      return false;
  return true;
};

} // namespace MATH