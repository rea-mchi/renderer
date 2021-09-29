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

// 相机和视口都对准中心的透视投影
// 由于数据坐标系范围是[-1,1]，转换后还要投射到屏幕坐标系，范围应为[0,width]
Vector3F PerspectiveProjectionTransformation(const Vector3F& vertex,
                                             const double camera_z,
                                             const double screen_z,
                                             const double screen_width,
                                             const double screen_height) {
  double coe = (camera_z - screen_z) / (camera_z - vertex.z);
  return Vector3F((vertex.x * coe + 1.) * screen_width / 2.,
                  (vertex.y * coe + 1.) * screen_height / 2., vertex.z);
}

// draw a triangle by bounding box method.
// 检查包围盒内所有像素，利用三角形重心坐标判断像素是否位于三角形内，如果是，就上色。
// 三角形重心坐标形如OP = (1-a_1-a_2)OA + a_1OB + a_2OC
// @param zbuffer size为像素个数的vector，用来记录各个像素当前最大z-value.
void TriangleByBB(const Vector3F& v0, const Vector3F& v1, const Vector3F& v2,
                  const Vector2F& vt0, const Vector2F& vt1, const Vector2F& vt2,
                  std::vector<double>& zbuffer, const TgaImage& texture,
                  const double camera_z, const double screen_z,
                  TgaImage* image) {
  // optimization possible?
  double xmin = std::floor(std::min(v0.x, std::min(v1.x, v2.x)));
  double xmax = std::ceil(std::max(v0.x, std::max(v1.x, v2.x)));
  double ymin = std::floor(std::min(v0.y, std::min(v1.y, v2.y)));
  double ymax = std::ceil(std::max(v0.y, std::max(v1.y, v2.y)));

  double dx = v1.x - v0.x;
  double dy = v1.y - v0.y;
  double dx2 = v2.x - v0.x;
  double dy2 = v2.y - v0.y;
  double a2_multiplier = dx * dy2 - dy * dx2;
  if (0 == a2_multiplier) {
    // The three vertices cannot represent a triangle. Maybe a line or a point.
    return;
  }
  // int width = image->width_;
  int width = image->GetWidth();
  for (int i = xmin; i <= xmax; ++i) {
    for (int j = ymin; j <= ymax; ++j) {
      double dx_p = i * 1. - v0.x;
      double dy_p = j * 1. - v0.y;
      // uv can multipled by dx*v_multiplier to reduce division calculation cost
      double a2 = (dy_p * dx - dx_p * dy) / a2_multiplier;
      double a1 = (dx_p - a2 * dx2) / dx;
      double a0 = 1. - a1 - a2;
      if (0 <= a0 && 0 <= a1 && 0 <= a2) {
        double z_p = a0 * v0.z + a1 * v1.z + a2 * v2.z;
        int pixel_index = j * width + i;
        if (z_p > zbuffer[pixel_index]) {
          zbuffer[pixel_index] = z_p;
          int u = static_cast<int>((a0 * vt0.x + a1 * vt1.x + a2 * vt2.x) *
                                   texture.GetWidth());
          int v = static_cast<int>((a0 * vt0.y + a1 * vt1.y + a2 * vt2.y) *
                                   texture.GetHeight());
          image->SetColor(i, j, texture.GetColor(u, v));
        }
      }
    }
  }
}

void DrawTriangle(const Vector3F& v0, const Vector3F& v1, const Vector3F& v2,
                  const Vector2F& vt0, const Vector2F& vt1, const Vector2F& vt2,
                  std::vector<double>& zbuffer, const TgaImage& texture,
                  const double camera_z, const double screen_z,
                  TgaImage* image) {
  if (!image) {
    std::cerr << "Null image pointer was past.\n";
    return;
  }
  if (camera_z <= screen_z) {
    std::cerr << "Incorrect camera z-position behind screen.\n";
    return;
  }

  int sc_width = image->GetWidth();
  int sc_height = image->GetHeight();

  Vector3F sc_v0 = PerspectiveProjectionTransformation(v0, camera_z, screen_z,
                                                       sc_width, sc_height);
  Vector3F sc_v1 = PerspectiveProjectionTransformation(v1, camera_z, screen_z,
                                                       sc_width, sc_height);
  Vector3F sc_v2 = PerspectiveProjectionTransformation(v2, camera_z, screen_z,
                                                       sc_width, sc_height);

  // TriangleByLineSwap(sc_v0.x, sc_v0.y, sc_v1.x, sc_v1.y, sc_v2.x, sc_v2.y,
  //                    image, white);

  TriangleByBB(sc_v0, sc_v1, sc_v2, vt0, vt1, vt2, zbuffer, texture, camera_z,
               screen_z, image);
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

  ObjModel* head = new ObjModel("/home/tea/my-renderer/obj/african_head.obj");
  TgaImage* texture = new TgaImage();
  TgaImage* image = new TgaImage(width, height, TgaImage::kRGB);
  texture->ReadTgaFile("/home/tea/my-renderer/obj/african_head_diffuse.tga");

  int pixel_num = width * height;
  std::vector<double> zbuffer(pixel_num, std::numeric_limits<double>::lowest());

  int face_num = head->GetFaceNum();
  for (int i = 0; i < face_num; ++i) {
    Vector3Int face = head->GetFaceVertices(i);
    Vector3Int face_texture = head->GetFaceVertexTextures(i);
    Vector3F v0 = head->GetVertex(face[0]);
    Vector3F v1 = head->GetVertex(face[1]);
    Vector3F v2 = head->GetVertex(face[2]);
    Vector2F uv0 = head->GetTexture(face_texture[0]);
    Vector2F uv1 = head->GetTexture(face_texture[1]);
    Vector2F uv2 = head->GetTexture(face_texture[2]);
    DrawTriangle(v0, v1, v2, uv0, uv1, uv2, zbuffer, *texture, 3., 1., image);
  }
  std::cerr << "Finish rasterization.\n";
  image->WriteTgaFile("african-head.tga", false, false, false);
  std::cerr << "Image IO finish.\n";
  return;
}
