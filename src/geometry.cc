#include "include/geometry.h"

#include <cmath>

// template <int N>
// Vec<double, N> vector::normalize(const Vec<double, N>& v) {
//   return v / v.norm();
// }

Vec3 vector::cross(const Vec3& v1, const Vec3& v2) {
  return Vec3{v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2],
              v1[0] * v2[1] - v1[1] * v2[0]};
}

Mat3 matrix::genIMat3() {
  Mat3 ret;
  for (int i = 0; i < 3; ++i) {
    ret(i, i) = 1;
  }
  return ret;
}

Mat4 matrix::genIMat4() {
  Mat4 ret;
  for (int i = 0; i < 4; ++i) {
    ret(i, i) = 1;
  }
  return ret;
}
