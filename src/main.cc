#include <array>
#include <cmath>
#include <iostream>
#include <limits>

#include "include/geometry.h"
#include "include/model.h"
#include "include/pipeline.h"
#include "include/tga_image.h"

// // draw line use Bresenham's algs
// // 每单位x增长的dy正是k的值，k =
// // abs(y1-y0)/(x1-x0)，当累积dy超过.5，就可以认为y进1。
// //
// 因为比较的始终是累积dy和.5，同乘2*(x1-x0)就可以把浮点数转化为整数，减小误差，也减少一次static_cast<double>(x1-x0)的操作，
// // 这就是k2dx的含义->k*2*dx.
// void DrawLine(int x0, int y0, int x1, int y1, TgaImage* image,
//               const TgaColor& color) {
//   if (!image) {
//     std::cerr << "Null image pointer was past.\n";
//     return;
//   }

//   bool xyswap = false;
//   if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
//     std::swap(x0, y0);
//     std::swap(x1, y1);
//     xyswap = true;
//   }
//   if (x1 < x0) {
//     std::swap(x0, x1);
//     std::swap(y0, y1);
//   }
//   int dx = x1 - x0;
//   if (0 == dx) {
//     image->SetColor(x0, y0, color);
//     return;
//   }
//   int k2dx = std::abs(y1 - y0) * 2;
//   int accumulated = -k2dx;
//   int y = y0;
//   for (int i = 0; i <= dx; ++i) {
//     accumulated += k2dx;
//     if (dx < accumulated) {
//       y += (y1 > y0 ? 1 : -1);
//       accumulated -= 2 * dx;
//     }
//     if (!xyswap) {
//       image->SetColor(x0 + i, y, color);
//     } else {
//       image->SetColor(y, x0 + i, color);
//     }
//   }
// }

const int width = 800;
const int height = 800;
Vec3 camera{0, 0, 2};
Vec3 gazeDir{0, 0, -1};
Vec3 viewupDir{0, 1, 0};

int main(int argc, char const* argv[]) {
  readModel("/home/tea/my-renderer/obj/african_head.obj");
  readTexture("/home/tea/my-renderer/obj/african_head_diffuse.tga");
  initImageBuffer(width, height, TgaImage::kRGB);
  setCamera(camera, gazeDir, viewupDir);
  setProjection(-1.5, 1.5, -1.5, 1.5, -1, -10.);
  // draw();
  Mat<2, 3> test;
  test(0, 0) = 1.3;
  test(1, 0) = 1.6;
  test(0, 1) = 4.5;
  test(1, 1) = 1.5;
  test(0, 2) = 4.9;
  test(1, 2) = 3.9;
  Vec3 bc{-1, -1, -1};
  std::cout << calcBarycentric(test, 3.0, 2.2, bc);
  printf("%f,%f,%f", bc[0], bc[1], bc[2]);
  save("african-head.tga");
}
