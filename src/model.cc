#include "include/model.h"

#include <algorithm>
#include <array>
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
  while (std::getline(in, line)) {
    if (line.empty() || '#' == line[0]) continue;
    std::istringstream iss(line);
    if (0 == line.compare(0, 2, "v ")) {
      // geometric vertices
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      Vector3F vertex;
      int i = 0;
      std::string value;
      while (iss && 3 > i) {
        iss >> value;
        vertex[i] = std::stod(value);
        ++i;
      }
      vertices_.push_back(vertex);
    } else if (0 == line.compare(0, 2, "vt")) {
      // texture coordinates
      continue;
    } else if (0 == line.compare(0, 2, "vn")) {
      // vertex normal
      // original data might not be unit vectors, so normalize manually
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      Vector3F normal;
      int i = 0;
      std::string value;
      while (iss && 3 > i) {
        iss >> value;
        normal[i] = std::stod(value);
        ++i;
      }
      normals_.push_back(vector3::Normalize(normal));
    } else if (0 == line.compare(0, 2, "f ")) {
      // polygonal face elemnt
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      Vector3Int face_vertices;
      Vector3Int face_normals;
      std::string vertex_data;
      int i = 0;
      while (3 > i) {
        iss >> vertex_data;
        std::array<std::string, 3> vertex_strs =
            SplitObjFaceVertexString(vertex_data);
        face_vertices[i] = std::stoi(vertex_strs[0]) - 1;
        face_normals[i] = std::stoi(vertex_strs[2]) - 1;
        ++i;
      }
      faces_.push_back(face_vertices);
      faces_normals_.push_back(face_normals);
    }
  }
  in.close();
}

ObjModel::~ObjModel() = default;

const Vector3F& ObjModel::GetVertex(int index) const {
  return vertices_[index];
}

const Vector3F& ObjModel::GetNormal(int index) const { return normals_[index]; }

const Vector3Int& ObjModel::GetFace(int index) const { return faces_[index]; }

std::size_t ObjModel::GetFaceNum() const { return faces_.size(); }

const Vector3Int& ObjModel::GetFaceNormals(int index) const {
  return faces_normals_[index];
}

std::array<std::string, 3> SplitObjFaceVertexString(
    const std::string& vertex_data) {
  std::array<std::string, 3> data;
  int len = vertex_data.length();
  int last = 0;
  int cur = 0;
  for (int i = 0; i < len; ++i) {
    if ('/' == vertex_data[i]) {
      data[cur] = vertex_data.substr(last, i - last);
      last = i + 1;
      cur++;
    }
  }
  data[cur] = vertex_data.substr(last);
  return data;
}
