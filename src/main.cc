#include <iostream>

#include "include/geometry.h"
#include "include/model.h"
#include "include/tga_image.h"

// draw line use Bresenham's algs
// 每单位x增长的dy正是k的值，k =
// abs(y1-y0)/(x1-x0)，当累积dy超过.5，就可以认为y进1。
// 因为比较的始终是累积dy和.5，同乘2*(x1-x0)就可以把浮点数转化为整数，减小误差，也减少一次static_cast<double>(x1-x0)的操作，
// 这就是k2dx的含义->k*2*dx.
void Line(int x0, int y0, int x1, int y1, TgaImage* image,
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

int width = 800;
int height = 800;
int main(int argc, char const* argv[]) {
  TgaColor white(255, 255, 255, 255);
  ObjModel head("/home/tea/my-renderer/obj/african_head.obj");
  TgaImage image(width, height, TgaImage::kRGB);
  for (const auto& face : head.faces_) {
    for (int i = 0; i < 3; ++i) {
      Vector3F v0 = head.vertices_[face[i]];
      Vector3F v1 = head.vertices_[face[(i + 1) % 3]];
      int x0 = (v0.x + 1.) * width / 2;
      int y0 = (v0.y + 1.) * height / 2;
      int x1 = (v1.x + 1.) * width / 2;
      int y1 = (v1.y + 1.) * height / 2;
      Line(x0, y0, x1, y1, &image, white);
    }
  }
  image.WriteTgaFile("out.tga", false, false, false);
  return 0;
}
