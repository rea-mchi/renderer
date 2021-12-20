#include "include/model.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "include/geometry.h"

ObjModel::ObjModel(const std::string& filename) {
  std::ifstream in;
  in.open(filename.c_str());
  if (!in.is_open()) {
    in.close();
    std::cerr << "Can't open file " << filename << " .\n";
    return;
  }
  std::string line;
  int i;
  std::string val;
  Vec3 buffer;
  Vec2 uv_buffer;
  Vec<int, 9> tri_buffer;
  while (std::getline(in, line)) {
    if (line.empty() || '#' == line[0]) continue;
    i = 0;
    std::istringstream iss(line);
    if (0 == line.compare(0, 2, "v ")) {
      // vertex
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      while (iss && 3 > i) {
        iss >> val;
        buffer[i++] = std::stod(val);
      }
      vertex_.push_back(buffer);
    } else if (0 == line.compare(0, 2, "vt")) {
      // uv
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      while (iss && 2 > i) {
        iss >> val;
        uv_buffer[i++] = std::stod(val);
      }
      uv_.push_back(uv_buffer);
    } else if (0 == line.compare(0, 2, "vn")) {
      // normal
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      while (iss && 3 > i) {
        iss >> val;
        buffer[i++] = std::stod(val);
      }
      normal_.push_back(buffer);
    } else if (0 == line.compare(0, 2, "f ")) {
      // triangle
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      handleTriStr(iss, tri_buffer);
      tri_vertex_.emplace_back((std::initializer_list<int>){
          tri_buffer[0], tri_buffer[3], tri_buffer[6]});
      tri_uv_.emplace_back((std::initializer_list<int>){
          tri_buffer[1], tri_buffer[4], tri_buffer[7]});
      tri_normal_.emplace_back((std::initializer_list<int>){
          tri_buffer[2], tri_buffer[5], tri_buffer[8]});
    }
  }
  in.close();
}

ObjModel::~ObjModel() = default;

const std::size_t ObjModel::vertexCount() const { return vertex_.size(); }
const Vec3& ObjModel::nthVertex(int n) const { return vertex_[n]; }
const Vec2& ObjModel::nthUv(int n) const { return uv_[n]; }
const Vec3& ObjModel::nthNormal(int n) const { return normal_[n]; }
const std::size_t ObjModel::triangleCount() const { return tri_vertex_.size(); }
const Vec3Int& ObjModel::nthTriVertexIndexs(int n) const {
  return tri_vertex_[n];
}
const Vec3Int& ObjModel::nthTriUvIndexs(int n) const { return tri_uv_[n]; }
const Vec3Int& ObjModel::nthTriNormalIndexs(int n) const {
  return tri_normal_[n];
}

void handleTriStr(std::istringstream& iss, Vec<int, 9>& buffer) {
  int n = 0;
  std::string s;
  std::string::size_type prev_pos, pos;
  int i = 0;
  while (3 > n++) {
    iss >> s;
    prev_pos = -1;
    pos = 0;
    while ((pos = s.find_first_of('/', prev_pos + 1)) != std::string::npos) {
      buffer[i++] = std::stoi(s.substr(prev_pos + 1, pos)) - 1;
      prev_pos = pos++;
    }
    buffer[i++] = std::stoi(s.substr(prev_pos + 1)) - 1;
  }
  assert(9 == i);
}
