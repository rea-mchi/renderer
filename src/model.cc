#include "include/model.h"

#include <algorithm>
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
    } else if (0 == line.compare(0, 2, "f ")) {
      // polygonal face elemnt
      iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      Vector3Int face;
      std::string vertex_data;
      int i = 0;
      while (3 > i) {
        iss >> vertex_data;
        std::vector<std::string> vertex = SplitObjFaceVertexString(vertex_data);
        face[i] = std::stoi(vertex[0]) - 1;
        ++i;
      }
      faces_.push_back(face);
    }
  }
  in.close();
}

std::vector<std::string> SplitObjFaceVertexString(
    const std::string& vertex_data) {
  std::vector<std::string> data;
  int len = vertex_data.length();
  int last = 0;
  for (int i = 0; i < len; ++i) {
    if ('/' == vertex_data[i]) {
      data.push_back(vertex_data.substr(last, i - last));
      last = i + 1;
    }
  }
  data.push_back(vertex_data.substr(last));
  return data;
}
