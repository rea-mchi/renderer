#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <cmath>
#include <iostream>
#include <vector>

template <typename T>
struct Vector3 {
  T x, y, z;
  Vector3() : x(0), y(0), z(0) {}
  Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
  Vector3(const Vector3& vector) : x(vector.x), y(vector.y), z(vector.z) {}

  Vector3<double> Normalize() {
    // use static assert to avoid non-numeric type?
    double length = std::sqrt(x * x + y * y + z * z);
    return Vector3(x / length, y / length, z / length);
  }

  T& operator[](const size_t index) {
    if (index < 0 || index > 2) {
      throw std::out_of_range("Invalid index!\n");
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
      throw std::out_of_range("Invalid index!\n");
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

template <typename T>
struct Vector2 {
  T x, y;
  Vector2() : x(0), y(0) {}
  Vector2(T x, T y) : x(x), y(y) {}
  Vector2(const Vector2& vector) : x(vector.x), y(vector.y) {}
  explicit Vector2(Vector3<T> vector3) : x(vector3.x), y(vector3.y) {}

  T& operator[](const size_t index) {
    if (index < 0 || index > 1) {
      throw std::out_of_range("Invalid index!\n");
    }
    if (0 == index) {
      return x;
    } else {
      return y;
    }
  }

  const T& operator[](const size_t index) const {
    if (index < 0 || index > 1) {
      throw std::out_of_range("Invalid index!\n");
    }
    if (0 == index) {
      return x;
    } else {
      return y;
    }
  }
};

using Vector2Int = Vector2<int>;
using Vector2F = Vector2<double>;

#endif  // GEOMETRY_H_
