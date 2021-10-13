#include <array>
#include <cmath>
#include <iostream>
#include <limits>

#include "include/geometry.h"
#include "include/model.h"
#include "include/shader.h"
#include "include/tga_image.h"

// draw line use Bresenham's algs
// 每单位x增长的dy正是k的值，k =
// abs(y1-y0)/(x1-x0)，当累积dy超过.5，就可以认为y进1。
// 因为比较的始终是累积dy和.5，同乘2*(x1-x0)就可以把浮点数转化为整数，减小误差，也减少一次static_cast<double>(x1-x0)的操作，
// 这就是k2dx的含义->k*2*dx.
void DrawLine(int x0, int y0, int x1, int y1, TgaImage* image,
              const TgaColor& color) {
  if (!image) {
    std::cerr << "Null image pointer was past.\n";
    return;
  }

  bool xyswap = false;
  if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    xyswap = true;
  }
  if (x1 < x0) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  int dx = x1 - x0;
  if (0 == dx) {
    image->SetColor(x0, y0, color);
    return;
  }
  int k2dx = std::abs(y1 - y0) * 2;
  int accumulated = -k2dx;
  int y = y0;
  for (int i = 0; i <= dx; ++i) {
    accumulated += k2dx;
    if (dx < accumulated) {
      y += (y1 > y0 ? 1 : -1);
      accumulated -= 2 * dx;
    }
    if (!xyswap) {
      image->SetColor(x0 + i, y, color);
    } else {
      image->SetColor(y, x0 + i, color);
    }
  }
}

// draw a triangle by line swap algs.
// Here use vertical line.
void TriangleByLineSwap(int x0, int y0, int x1, int y1, int x2, int y2,
                        TgaImage* image, const TgaColor& color) {
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  if (x0 > x2) {
    std::swap(x0, x2);
    std::swap(y0, y2);
  }
  if (x1 > x2) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }
  double k01 = x0 == x1 ? 0 : (y1 - y0) * 1. / (x1 - x0);
  double k02 = x0 == x2 ? 0 : (y2 - y0) * 1. / (x2 - x0);
  double k12 = x1 == x2 ? 0 : (y2 - y1) * 1. / (x2 - x1);
  for (int i = x0; i <= x1; ++i) {
    int y01 = y0 + k01 * (i - x0);
    int y02 = y0 + k02 * (i - x0);
    DrawLine(i, y01, i, y02, image, color);
  }
  for (int i = x1; i <= x2; ++i) {
    int y12 = y1 + k12 * (i - x1);
    int y02 = y0 + k02 * (i - x0);
    DrawLine(i, y12, i, y02, image, color);
  }
}

void Rasterization(const std::array<Vector4, 3>& v,
                   std::vector<double>& zbuffer, const double width,
                   IShader* shader) {
  // use BB
  // optimization possible?
  double xmin = std::floor(std::min(v[0][0], std::min(v[1][0], v[2][0])));
  double xmax = std::ceil(std::max(v[0][0], std::max(v[1][0], v[2][0])));
  double ymin = std::floor(std::min(v[0][1], std::min(v[1][1], v[2][1])));
  double ymax = std::ceil(std::max(v[0][1], std::max(v[1][1], v[2][1])));

  // check three vertices at one line
  // 123 abc
  Vector4 ab = v[1] - v[0];
  Vector4 bc = v[2] - v[1];
  if (1e-6 > vector_m::Cross(ab, bc).Norm()) {
    return;
  }

  for (int i = xmin; i <= xmax; ++i) {
    for (int j = ymin; j <= ymax; ++j) {
      double ap_x = i * 1. - v[0][0];
      double ap_y = j * 1. - v[0][1];
      Vector3 x_component{ab[0], bc[0], ap_x};
      Vector3 y_component{ab[1], bc[1], ap_y};
      Vector3 product = vector_m::Cross(x_component, y_component);
      product = product / (-product[2]);
      if (1 >= product[0] && 0 <= product[1] && product[0] >= product[1]) {
        Vector3 barycentric{1. - product[0], product[0] - product[1],
                            product[1]};
        double interpo_z =
            vector_m::Dot(barycentric, Vector3{v[0][2], v[1][2], v[2][2]});
        int pixel_index = j * width + i;
        if (interpo_z > zbuffer[pixel_index]) {
          zbuffer[pixel_index] = interpo_z;
          shader->fragment_process(Vector2Int{i, j}, barycentric);
        }
      }
    }
  }
}

const int width = 800;
const int height = 800;
Vector3 camera_pos{-2, 0, 2};
Vector3 gaze_dir{1, 0, -1};
Vector3 up{0, 1, 0};

int main(int argc, char const* argv[]) {
  ObjModel* head = new ObjModel("/home/tea/my-renderer/obj/african_head.obj");
  TgaImage* image = new TgaImage(width, height, TgaImage::kRGB);
  TextureShader shader("/home/tea/my-renderer/obj/african_head_diffuse.tga");
  shader.RegisterCanvas(image);
  shader.LookAt(camera_pos, gaze_dir, up);
  shader.Projection(-1.5);
  shader.SetViewPort(width, height);

  int pixel_num = width * height;
  std::vector<double> zbuffer(pixel_num, std::numeric_limits<double>::lowest());

  int face_num = head->GetFaceNum();
  std::array<Vector4, 3> vertices;
  std::array<Vector2, 3> textures;
  for (int i = 0; i < face_num; ++i) {
    Vector3Int face = head->GetFaceVertices(i);
    Vector3Int face_texture = head->GetFaceVertexTextures(i);
    for (int i = 0; i < 3; i++) {
      vertices[i] = shader.vertex_process(
          vector_m::HomogeneousCoords(head->GetVertex(face[i])));
      shader.SetTextureCoordinates(head->GetTexture(face_texture[i]), i);
    }
    Rasterization(vertices, zbuffer, width, &shader);
  }
  shader.UnregisterCanvas();
  image->WriteTgaFile("african-head.tga", false, false, false);
  delete head;
  delete image;
}
