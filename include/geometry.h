#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <iostream>
#include <vector>

template <typename T>
struct Vector3 {
  T x, y, z;
  Vector3() : x(0), y(0), z(0) {}
  Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

  T& operator[](const size_t index) {
    if (index < 0 || index > 2) {
      throw std::out_of_range("Invalid index!");
    }
    if (0 == index) {
      return x;
    } else if (1 == index) {
      return y;
    } else {
      return z;
    }
  }

  const T& operator[](const size_t index) const {
    if (index < 0 || index > 2) {
      throw std::out_of_range("Invalid index!");
    }
    if (0 == index) {
      return x;
    } else if (1 == index) {
      return y;
    } else {
      return z;
    }
  }
};

using Vector3Int = Vector3<int>;
using Vector3F = Vector3<double>;

#endif  // GEOMETRY_H_
