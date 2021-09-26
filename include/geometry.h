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

namespace vector3 {
template <typename T>
Vector3F Normalize(Vector3<T> vec) {
  double norm = std::sqrt(x * x + y * y + z * z);
  return Vector3F(x / norm, y / norm, z / norm);
}

template <typename T>
Vector3<T> Cross(Vector3<T> a, Vector3<T> b) {
  return Vector3<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x);
}

template <typename T>
T Dot(Vector3<T> a, Vector3<T> b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
}  // namespace vector3

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

namespace vector2 {
template <typename T>
Vector3<T> Cross(Vector2<T> a, Vector2<T> b) {
  return Vector3<T>(0, 0, a.x * b.y - a.y * b.x);
}

template <typename T>
T Dot(Vector2<T> a, Vector2<T> b) {
  return a.x * b.x + a.y * b.y;
}
}  // namespace vector2

#endif  // GEOMETRY_H_
