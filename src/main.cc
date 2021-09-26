#include <algorithm>
#include <iostream>
#include <limits>

#include "include/geometry.h"
#include "include/model.h"
#include "include/tga_image.h"

TgaColor white(255, 255, 255, 255);
TgaColor red(255, 0, 0, 255);

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

// draw a triangle by bounding box method.
// 检查包围盒内所有像素，利用三角形重心坐标判断像素是否位于三角形内，如果是，就上色。
// 三角形重心坐标形如OP = (1-u-v)OA + uOB + vOC
// @param zbuffer size为像素个数的vector，用来记录各个像素当前最大z-value.
void TriangleByBB(const Vector3F& vertex0, const Vector3F& vertex1,
                  const Vector3F& vertex2, std::vector<double>& zbuffer,
                  TgaImage* image, const TgaColor& color) {
  // optimization possible?
  double xmin = std::floor(std::min(vertex0.x, std::min(vertex1.x, vertex2.x)));
  double xmax = std::ceil(std::max(vertex0.x, std::max(vertex1.x, vertex2.x)));
  double ymin = std::floor(std::min(vertex0.y, std::min(vertex1.y, vertex2.y)));
  double ymax = std::ceil(std::max(vertex0.y, std::max(vertex1.y, vertex2.y)));

  double dx = vertex1.x - vertex0.x;
  double dy = vertex1.y - vertex0.y;
  double dx2 = vertex2.x - vertex0.x;
  double dy2 = vertex2.y - vertex0.y;
  double v_multiplier = dx * dy2 - dy * dx2;
  if (0 == v_multiplier) {
    // The three vertices cannot represent a triangle. Maybe a line or a point.
    // Note that implicit conversion from double to int!
    if (0 != dx)
      DrawLine(vertex0.x, vertex0.y, vertex1.x, vertex1.y, image, color);
    if (0 != dx2)
      DrawLine(vertex0.x, vertex0.y, vertex2.x, vertex2.y, image, color);
    if (vertex1.x != vertex2.x)
      DrawLine(vertex1.x, vertex1.y, vertex2.x, vertex2.y, image, color);
    return;
  }
  int width = image->width_;
  for (int i = xmin; i <= xmax; ++i) {
    for (int j = ymin; j <= ymax; ++j) {
      double dx_p = i * 1. - vertex0.x;
      double dy_p = j * 1. - vertex0.y;
      double v = (dy_p * dx - dx_p * dy) / v_multiplier;
      double u = (dx_p - v * dx2) / dx;
      if (0 <= u && 0 <= v && 0 <= 1. - u - v) {
        double z_p = (1. - u - v) * vertex0.z + u * vertex1.z + v * vertex2.z;
        int pixel_index = j * width + i;
        if (z_p > zbuffer[pixel_index]) {
          zbuffer[pixel_index] = z_p;
          image->SetColor(i, j, color);
        }
        image->SetColor(i, j, color);
      }
    }
  }
}

void DrawTriangle(const Vector3F& vertex0, const Vector3F& vertex1,
                  const Vector3F& vertex2, std::vector<double>& zbuffer,
                  TgaImage* image, const TgaColor& color) {
  if (!image) {
    std::cerr << "Null image pointer was past.\n";
    return;
  }
  // TriangleByLineSwap(vertex0.x, vertex0.y, vertex1.x, vertex1.y, vertex2.x,
  //                    vertex2.y, image, color);

  TriangleByBB(vertex0, vertex1, vertex2, zbuffer, image, color);
}

void readModel();

int main(int argc, char const* argv[]) {
  // TgaImage image(100, 100, TgaImage::kRGB);
  // Vector2Int a(10, 15), b(50, 40), c(70, 60);
  // DrawTriangle(a, b, c, &image, white);
  // Vector2Int d(70, 15), e(40, 5);
  // DrawTriangle(b, d, c, &image, red);
  // image.WriteTgaFile("out.tga", false, false, false);
  readModel();
  return 0;
}

void readModel() {
  int width = 800;
  int height = 800;

  ObjModel head("/home/tea/my-renderer/obj/african_head.obj");
  TgaImage image(width, height, TgaImage::kRGB);
  int pixel_num = width * height;
  std::vector<double> zbuffer(pixel_num, std::numeric_limits<double>::lowest());
  int face_num = head.GetFaceNum();
  for (int i = 0; i < face_num; ++i) {
    Vector3Int face = head.GetFace(i);
    Vector3F v0 = head.GetVertex(face[0]);
    Vector3F v1 = head.GetVertex(face[1]);
    Vector3F v2 = head.GetVertex(face[2]);
    Vector3F sv0((v0.x + 1.) * width / 2., (v0.y + 1.) * height / 2., v0.z);
    Vector3F sv1((v1.x + 1.) * width / 2., (v1.y + 1.) * height / 2., v1.z);
    Vector3F sv2((v2.x + 1.) * width / 2., (v2.y + 1.) * height / 2., v2.z);
    Vector3F vec01(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
    Vector3F vec02(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
    Vector3F n = vector3::Cross(vec02, vec01);
    double intensity = vector3::Normalize(n).z * (-1);

    if (intensity > 0) {
      DrawTriangle(
          sv0, sv1, sv2, zbuffer, &image,
          TgaColor(255 * intensity, 255 * intensity, 255 * intensity, 255));
    }
  }
  image.WriteTgaFile("african-head.tga", false, false, false);
}
