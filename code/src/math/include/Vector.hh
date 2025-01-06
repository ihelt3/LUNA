/*------------------------------------------------------------------------*\
**  
**  @file:      Vector.hh
**
**  @author:    Tom-Robin Teschner (cfd.university)
**
**  @brief:     header file for Vector class
**
\*------------------------------------------------------------------------*/

#ifndef _VECTOR_HH_
#define _VECTOR_HH_

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

namespace MATH {

class Vector {
public:
  using VectorType = std::vector<double>;

public:
  Vector();
  Vector(const unsigned &size);
  Vector(const unsigned &size, double value);
  Vector(const std::vector<double> &vector);

  unsigned size() const;

  // Member functions
  Vector transpose();
  double getL2Norm() const;
  
  // Get methods
  VectorType get_vector() const { return _vector; };

  // Operator Overloading
  const double &operator[](unsigned index) const;
  double &operator[](unsigned index);

  Vector operator+(const Vector &other) const;
  Vector operator-(const Vector &other) const;

  // Vector &operator*(const double &scaleFactor);
  friend Vector operator*(const double &scaleFactor, Vector vector);
  friend Vector operator*(Vector vector, const double &scaleFactor);  
  double operator*(const Vector &other) const;

  // Vector &operator/(const double &divisor);
  friend Vector operator/(Vector vector, const double &divisor);
  friend std::ostream &operator<<(std::ostream &out, const Vector &vector);

  // Vector ==operator()
  bool operator==(const Vector &other) const;

private:
  VectorType _vector;
  bool _isRowVector = false;
};

} // namespace linearAlgebraLib

#endif  // _VECTOR_HH_