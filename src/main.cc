#include <algorithm>
#include <iostream>

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
// 重心坐标计算式: OP = (1-u-v)OA + uOB + vOC.
void TriangleByBB(int x0, int y0, int x1, int y1, int x2, int y2,
                  TgaImage* image, const TgaColor& color) {
  int xmin = std::min(x0, std::min(x1, x2));  // optimization possible?
  int xmax = std::max(x0, std::max(x1, x2));
  int ymin = std::min(y0, std::min(y1, y2));
  int ymax = std::max(y0, std::max(y1, y2));

  int dy = y1 - y0;
  int dx = x1 - x0;
  int dx2 = x2 - x0;
  int multiplier = (x0 - x2) * dy + (y2 - y0) * dx;
  if (0 == multiplier) {
    // The three vertices cannot represent a triangle. Maybe a line or a point.
    if (0 != dx) DrawLine(x0, y0, x1, y1, image, color);
    if (0 != dx2) DrawLine(x0, y0, x2, y2, image, color);
    if (x1 != x2) DrawLine(x1, y1, x2, y2, image, color);
  }
  for (int i = xmin; i <= xmax; ++i) {
    for (int j = ymin; j <= ymax; ++j) {
      int dytemp = j - y0;
      int dxtemp = i - x0;
      double v = (dytemp * dx - dxtemp * dy) * 1. / multiplier;
      double u = (dxtemp - v * dx2) / dx;
      if (0 <= u && 1 >= u && 0 <= v && 1 >= v && 1 >= (u + v)) {
        image->SetColor(i, j, color);
      }
    }
  }
}

void DrawTriangle(const Vector2Int& vertex0, const Vector2Int& vectex1,
                  const Vector2Int& vertex2, TgaImage* image,
                  const TgaColor& color) {
  if (!image) {
    std::cerr << "Null image pointer was past.\n";
    return;
  }
  // TriangleByLineSwap(vertex0.x, vertex0.y, vectex1.x, vectex1.y, vertex2.x,
  //                    vertex2.y, image, color);
  TriangleByBB(vertex0.x, vertex0.y, vectex1.x, vectex1.y, vertex2.x, vertex2.y,
               image, color);
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
  int face_num = head.GetFaceNum();
  for (int i = 0; i < face_num; ++i) {
    Vector3Int face = head.GetFace(i);
    Vector3F v0 = head.GetVertex(face[0]);
    Vector3F v1 = head.GetVertex(face[1]);
    Vector3F v2 = head.GetVertex(face[2]);
    Vector2Int sv0((v0.x + 1.) * width / 2., (v0.y + 1.) * height / 2.);
    Vector2Int sv1((v1.x + 1.) * width / 2., (v1.y + 1.) * height / 2.);
    Vector2Int sv2((v2.x + 1.) * width / 2., (v2.y + 1.) * height / 2.);
    DrawTriangle(sv0, sv1, sv2, &image,
                 TgaColor(rand() % 255, rand() % 255, rand() % 255, 255));
  }
  image.WriteTgaFile("african-head.tga", false, false, false);
}
