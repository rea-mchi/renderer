#ifndef MODEL_H_
#define MODEL_H_

#include <sstream>
#include <string>
#include <vector>

#include "include/geometry.h"

// .obj文件
class ObjModel {
 public:
  explicit ObjModel(const std::string& filename);
  ObjModel(const ObjModel& model) = delete;
  ObjModel& operator=(const ObjModel& rhs) = delete;
  ~ObjModel();

  const std::size_t vertexCount() const;
  const Vec3& nthVertex(int n) const;
  const Vec2& nthUv(int n) const;
  const Vec3& nthNormal(int n) const;
  const std::size_t triangleCount() const;
  const Vec3Int& nthTriVertexIndexs(int n) const;
  const Vec3Int& nthTriUvIndexs(int n) const;
  const Vec3Int& nthTriNormalIndexs(int n) const;

 private:
  std::vector<Vec3> vertex_;
  std::vector<Vec2> uv_;
  std::vector<Vec3> normal_;
  std::vector<Vec3Int> tri_vertex_;
  std::vector<Vec3Int> tri_uv_;
  std::vector<Vec3Int> tri_normal_;
};

// 用于处理.obj中面信息的字符串，只考虑完整形如f v1/vt1/vn1 v2/vt2/vn2
// v3/vt3/vn3 iss包裹的是"v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3"，存储在buffer内
void handleTriStr(std::istringstream& iss, Vec<int, 9>& buffer);

#endif  // MODEL_H_
