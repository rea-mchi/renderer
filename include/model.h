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
  Vector3 GetVertex(int index) const;
  Vector2 GetTexture(int index) const;
  Vector3 GetNormal(int index) const;
  std::size_t GetFaceNum() const;
  Vector3Int GetFaceVertices(int index) const;
  Vector3Int GetFaceVertexTextures(int index) const;
  Vector3Int GetFaceVertexNormals(int index) const;

 private:
  std::vector<Vector3> vertices_;
  std::vector<Vector2> textures_;
  std::vector<Vector3> normals_;
  std::vector<Vector3Int> faces_vertices_;
  std::vector<Vector3Int> faces_vertex_textures_;
  std::vector<Vector3Int> faces_vertex_normals_;
};

// Wavefront.obj的文件格式中，面元数据行格式为f v1/vt1/vn1 v2/vt2/vn2
// v3/vt3/vn3。
// 该函数负责将形如v1/vt/vn1的字符串转化为{v1,vt,vn1},存储到参数数组中。
// @param vertex_data 形如v1/vt/vn1的字符串。
// @param array 存储{v1,vt,vn1}，容量为3的数组
// 使用数组减少堆内存碎片。
void SplitObjFaceVertexStr(const std::string& vertex_data_str,
                           std::array<std::string, 3>& array);

#endif  // MODEL_H_
