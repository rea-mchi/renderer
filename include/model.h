#ifndef MODEL_H_
#define MODEL_H_

#include <string>
#include <vector>

#include "include/geometry.h"

// 读取Wavefront.obj文件并存储其中数据
class ObjModel {
 public:
  std::vector<Vector3F> vertices_;
  std::vector<Vector3Int> faces_;

  explicit ObjModel(const std::string& filename);
};

// Wavefront.obj的文件格式中，面元数据行格式为f v1/vt1/vn1 v2/vt2/vn2
// v3/vt3/vn3。 该函数负责将形如v1/vt/vn1的字符串转化为{v1,vt,vn1}的矢量形式。
// @param vertex_data 形如v1/vt/vn1的字符串。
std::vector<std::string> SplitObjFaceVertexString(
    const std::string& vertex_data);

#endif  // MODEL_H_
