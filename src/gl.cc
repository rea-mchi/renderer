#include "include/gl.h"

#include "include/geometry.h"

// @param gaze_direction 使用右手坐标系，因此表示-z方向
SMatrix4 CameraTransM(const Vector3& camera_pos, const Vector3& gaze_direction,
                      const Vector3& viewup) {
  // calculate local coordinates uvw
  Vector3 w = vector_m::Normalize(gaze_direction * -1.);
  Vector3 u = vector_m::Normalize(vector_m::Cross(viewup, w));
  Vector3 v = vector_m::Cross(w, u);
  SMatrix4 rTransM = matrix_m::IMatrix4();

  SMatrix4 rRotM = matrix_m::IMatrix4();
  for (int i = 0; i < 3; ++i) {
    rTransM(i, 3) = -camera_pos[i];
    rRotM(0, i) = u[i];
    rRotM(1, i) = v[i];
    rRotM(2, i) = w[i];
  }

  return rRotM * rTransM;
}

// 投影变换
// 看向-z方向，所以near > far
SMatrix4 ProjectionM(double left, double right, double bottom, double top,
                     double far, double near, bool perspective) {
  SMatrix4 m = matrix_m::IMatrix4();
  m(0, 0) = 2. / (right - left);
  m(0, 3) = (right + left) / (left - right);
  m(1, 1) = 2. / (top - bottom);
  m(1, 3) = (top + bottom) / (bottom - top);
  m(2, 2) = 2. / (near - far);
  m(2, 3) = (near + far) / (far - near);
  if (!perspective) {
    return m;
  } else {
    SMatrix4 m_per;
    m_per(0, 0) = near;
    m_per(1, 1) = near;
    m_per(2, 2) = near + far;
    m_per(2, 3) = -near * far;
    m_per(3, 2) = 1;
    return m * m_per;
  }
}

SMatrix4 ViewportTransM(const double screen_width, const double screen_height) {
  SMatrix4 m = matrix_m::IMatrix4();
  m(0, 0) = screen_width * .5;
  m(0, 3) = screen_width * .5 - .5;
  m(1, 1) = screen_height * .5;
  m(1, 3) = screen_height * .5 - .5;
  return m;
}
