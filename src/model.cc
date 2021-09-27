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
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      Vector2F texture;
      int i = 0;
      std::string value;
      while (iss && 2 > i) {
        iss >> value;
        texture[i] = std::stod(value);
        ++i;
      }
      textures_.push_back(texture);
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
      Vector3Int face_vertex_textures;
      Vector3Int face_vertex_normals;
      std::string vertex_data_str;
      int i = 0;
      std::array<std::string, 3> array;
      while (3 > i) {
        iss >> vertex_data_str;
        SplitObjFaceVertexStr(vertex_data_str, array);
        face_vertices[i] = std::stoi(array[0]) - 1;
        face_vertex_textures[i] = std::stoi(array[1]) - 1;
        face_vertex_normals[i] = std::stoi(array[2]) - 1;
        ++i;
      }
      faces_vertices_.push_back(face_vertices);
      faces_vertex_textures_.push_back(face_vertex_textures);
      faces_vertex_normals_.push_back(face_vertex_normals);
    }
  }
  in.close();
}

ObjModel::~ObjModel() = default;

const Vector3F& ObjModel::GetVertex(int index) const {
  return vertices_[index];
}

const Vector2F& ObjModel::GetTexture(int index) const {
  return textures_[index];
}

const Vector3F& ObjModel::GetNormal(int index) const { return normals_[index]; }

const Vector3Int& ObjModel::GetFaceVertices(int index) const {
  return faces_vertices_[index];
}

std::size_t ObjModel::GetFaceNum() const { return faces_vertices_.size(); }

const Vector3Int& ObjModel::GetFaceVertexTextures(int index) const {
  return faces_vertex_textures_[index];
}
const Vector3Int& ObjModel::GetFaceVertexNormals(int index) const {
  return faces_vertex_normals_[index];
}

void SplitObjFaceVertexStr(const std::string& vertex_data_str,
                           std::array<std::string, 3>& array) {
  int len = vertex_data_str.length();
  int last = 0;
  int cur = 0;
  for (int i = 0; i < len; ++i) {
    if ('/' == vertex_data_str[i]) {
      array[cur] = vertex_data_str.substr(last, i - last);
      last = i + 1;
      cur++;
    }
  }
  array[cur] = vertex_data_str.substr(last);
  return;
}
