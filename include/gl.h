#ifndef GL_H_
#define GL_H_

#include "include/geometry.h"

// 设置相机相关参数，返回将世界坐标转换为相机坐标的矩阵mCamera
SMatrix4 CameraTransM(const Vector3& camera_pos, const Vector3& gaze_direction,
                      const Vector3& viewup);
// 返回投影变换矩阵，将实际几何大小转换到[-1,1]^3空间
// @param perspective 如果true，额外计算透视矩阵
SMatrix4 ProjectionM(double left, double right, double bottom, double top,
                     double far, double near, bool perspective);
// 返回视口变换矩阵
SMatrix4 ViewportTransM(const double screen_width, const double screen_height);

#endif  // GL_H_
