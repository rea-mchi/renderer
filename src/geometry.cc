#include "include/geometry.h"

Vector3 vector_m::Cross(const Vector2& v1, const Vector2& v2) {
  return Vector3{0, 0, v1[0] * v2[1] - v1[1] * v2[0]};
}
Vector3 vector_m::Cross(const Vector3& v1, const Vector3& v2) {
  return Vector3{v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2],
                 v1[0] * v2[1] - v1[1] * v2[0]};
}
Vector3 vector_m::Cross(const Vector4& v1, const Vector4& v2) {
  return Vector3{v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2],
                 v1[0] * v2[1] - v1[1] * v2[0]};
}

Vector4 vector_m::HomogeneousCoords(const Vector3& vector3) {
  return Vector4{vector3[0], vector3[1], vector3[2], 1.};
}

SMatrix3 matrix_m::IMatrix3() {
  SMatrix3 m;
  for (int i = 0; i < 3; ++i) {
    m(i, i) = 1;
  }
  return m;
}

SMatrix4 matrix_m::IMatrix4() {
  SMatrix4 m;
  for (int i = 0; i < 4; ++i) {
    m(i, i) = 1;
  }
  return m;
}
