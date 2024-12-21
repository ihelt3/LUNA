/*------------------------------------------------------------------------*\
**  
**  @file:      Vector.hh
**
**  @author:    Tom-Robin Teschner (cfd.university)
**
**  @brief:     header file for Vector class
**
\*------------------------------------------------------------------------*/
/*
Copyright 2023 Tom-Robin Teschner (cfd.university)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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