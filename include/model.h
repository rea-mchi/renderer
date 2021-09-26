#ifndef MODEL_H_
#define MODEL_H_

#include <array>
#include <string>
#include <vector>

#include "include/geometry.h"

// 读取Wavefront.obj文件并存储其中数据.
// vertices_ 几何顶点坐标;
// normals_ 顶点法线，单位向量;
// faces_ 三角面元 存储顶点index;
// faces_normals_ 三角面元每个顶点法线的index;
class ObjModel {
 public:
  explicit ObjModel(const std::string& filename);
  ~ObjModel();
  Vector3F GetVertex(int index) const;
  Vector3F GetNormal(int index) const;
  Vector3Int GetFace(int index) const;
  std::size_t GetFaceNum() const;
  Vector3Int GetFaceNormals(int index) const;

 private:
  std::vector<Vector3F> vertices_;
  std::vector<Vector3F> normals_;
  std::vector<Vector3Int> faces_;
  std::vector<Vector3Int> faces_normals_;
};

// Wavefront.obj的文件格式中，面元数据行格式为f v1/vt1/vn1 v2/vt2/vn2
// v3/vt3/vn3。 该函数负责将形如v1/vt/vn1的字符串转化为{v1,vt,vn1}的数组形式。
// @param vertex_data 形如v1/vt/vn1的字符串。
// 使用数组减少堆内存碎片。
std::array<std::string, 3> SplitObjFaceVertexString(
    const std::string& vertex_data);

#endif  // MODEL_H_
