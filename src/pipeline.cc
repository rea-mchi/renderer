#include "include/pipeline.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

#include "include/geometry.h"
#include "include/model.h"
#include "include/tga_image.h"

ObjModel* model = nullptr;
TgaImage* texture = nullptr;
TgaImage* buffer = nullptr;

Vec3 lightPos;
TgaColor lightColor;

Mat4 modelM = matrix::genIMat4();
Mat4 viewM = matrix::genIMat4();
Mat4 projectionM = matrix::genIMat4();
Mat4 viewportM = matrix::genIMat4();

Mat<4, 3> vertex;
Mat<2, 3> uv;
Mat<3, 3> normal;

// rasterization
double xmin, xmax, ymin, ymax;
// dont't need _01 because we can get gamma by 1. - alpha - beta
// double cp01, cp12, cp20;  // cross_product cp01=x0*y1-x1*y0
double cp12, cp20;
// double f01, f12, f20;  // f01(x2,y2) f12(x0,y0), f20(x1,y1)
double f12, f20;
Vec3 barycentric;

std::vector<double> zbuffer;

void* shader = nullptr;

void readModel(const std::string& filename) { model = new ObjModel(filename); }

void readTexture(const std::string& filename) {
  texture = new TgaImage();
  texture->ReadTgaFile(filename);
}

void initImageBuffer(const int w, const int h, TgaImage::Format format) {
  buffer = new TgaImage(w, h, format);

  // viewport set
  viewportM(0, 0) = w / 2;
  viewportM(0, 3) = w / 2 - .5;
  viewportM(1, 1) = h / 2;
  viewportM(1, 3) = h / 2 - .5;

  // zbuffer
  zbuffer.reserve(w * h);
  zbuffer.resize(w * h, std::numeric_limits<double>::lowest());
}

void setPointLight(const Vec3& pos, const TgaColor& lightc) {
  lightPos = pos;
  lightColor = lightc;
}

void setCamera(const Vec3& pos, const Vec3& gazeDir, const Vec3& viewupDir) {
  Vec3 w = vector::normalize(gazeDir) * (-1);
  Vec3 u = vector::normalize(vector::cross(viewupDir, w));
  Vec3 v = vector::cross(w, u);
  Mat4 m1;                       // rotation
  Mat4 m2 = matrix::genIMat4();  // anti-translation
  m1.replaceNthRow(0, u);
  m1.replaceNthRow(1, v);
  m1.replaceNthRow(2, w);
  m1(3, 3) = 1;
  m2.replaceNthCol(3, pos * (-1));
  viewM = m1 * m2;
}

void setProjection(const double l, const double r, const double b,
                   const double t, const double n, const double f,
                   const bool perspective) {
  assert(std::abs(r - l) > std::numeric_limits<double>::epsilon() &&
         std::abs(t - b) > std::numeric_limits<double>::epsilon() &&
         std::abs(f - n) > std::numeric_limits<double>::epsilon());
  Mat<4, 4> orth = matrix::genIMat4();
  Mat<4, 4> perp = matrix::genIMat4();
  orth(0, 0) = 2. / (r - l);
  orth(1, 1) = 2. / (t - b);
  orth(2, 2) = 2. / (n - f);
  Vec3 v{(r + l) / (l - r), (t + b) / (t - b), (n + f) / (f - n)};
  orth.replaceNthCol(3, v);
  if (perspective) {
    // clip
  }
  projectionM = orth * perp;
}

void modelProcess() { vertex = modelM * vertex; }
void viewProcess() { vertex = viewM * vertex; }
void projecionProcess() { vertex = projectionM * vertex; }
void viewportProcess() { vertex = viewportM * vertex; }

void rasterization() {
  xmin = std::numeric_limits<int>::max();
  xmax = 0;
  ymin = xmin;
  ymax = 0;
  for (int n = 0; n < 3; ++n) {
    xmin = std::min(std::floor(vertex(0, n)), xmin);
    xmax = std::max(std::ceil(vertex(0, n)), xmax);
    ymin = std::min(std::floor(vertex(1, n)), ymin);
    ymax = std::max(std::ceil(vertex(1, n)), ymax);
  }
  // cp01 = vertex(0, 0) * vertex(1, 1) - vertex(0, 1) * vertex(1, 0);
  cp12 = vertex(0, 1) * vertex(1, 2) - vertex(0, 2) * vertex(1, 1);
  cp20 = vertex(0, 2) * vertex(1, 0) - vertex(0, 0) * vertex(1, 2);
  // f01 = (vertex(1, 0) - vertex(1, 1)) * vertex(0, 2) +
  //       (vertex(0, 1) - vertex(0, 0)) * vertex(1, 2) + cp01;
  // if (std::fabs(f01) <= std::numeric_limits<double>::epsilon()) return;
  f12 = (vertex(1, 1) - vertex(1, 2)) * vertex(0, 0) +
        (vertex(0, 2) - vertex(0, 1)) * vertex(1, 0) + cp12;
  if (std::fabs(f12) <= std::numeric_limits<double>::epsilon()) return;
  f20 = (vertex(1, 2) - vertex(1, 0)) * vertex(0, 1) +
        (vertex(0, 0) - vertex(0, 2)) * vertex(1, 1) + cp20;
  if (std::fabs(f20) <= std::numeric_limits<double>::epsilon()) return;

  double coe = 0.0;
  Vec2 ipuv;  // uv interpolated
  for (double x = xmin; x <= xmax; ++x) {
    for (double y = ymin; y <= ymax; ++y) {
      coe = ((vertex(1, 1) - vertex(1, 2)) * x +
             (vertex(0, 2) - vertex(0, 1)) * y + cp12) /
            f12;
      if (0 > coe) continue;
      barycentric[0] = coe;
      coe = (vertex(1, 2) - vertex(1, 0)) * x +
            (vertex(0, 0) - vertex(0, 2)) * y + cp20;
      if (0 > coe) continue;
      barycentric[1] = coe;
      coe = 1. - barycentric[0] - barycentric[1];
      if (0 > coe) continue;
      barycentric[2] = coe;
      if (!depthTest(static_cast<int>(x), static_cast<int>(y))) continue;
      if (shader) {
        /* code */
      } else {
        if (texture) {
          // texture mapping
          ipuv = uv * barycentric;
          TgaColor c = texture->getColor(
              static_cast<int>(ipuv[0] * texture->GetWidth()),
              static_cast<int>(ipuv[1]) * texture->GetHeight());
          buffer->setColor(static_cast<int>(x), static_cast<int>(y), c);
        } else {
        }
      }
    }
  }
};

int calcBarycentric(const Mat<2, 3>& vertex, const double x, const double y,
                    Vec3& bc) {
  // cp01 = vertex(0, 0) * vertex(1, 1) - vertex(0, 1) * vertex(1, 0);
  cp12 = vertex(0, 1) * vertex(1, 2) - vertex(0, 2) * vertex(1, 1);
  cp20 = vertex(0, 2) * vertex(1, 0) - vertex(0, 0) * vertex(1, 2);
  // f01 = (vertex(1, 0) - vertex(1, 1)) * vertex(0, 2) +
  //       (vertex(0, 1) - vertex(0, 0)) * vertex(1, 2) + cp01;
  // if (std::fabs(f01) <= std::numeric_limits<double>::epsilon()) return;
  f12 = (vertex(1, 1) - vertex(1, 2)) * vertex(0, 0) +
        (vertex(0, 2) - vertex(0, 1)) * vertex(1, 0) + cp12;
  if (std::fabs(f12) <= std::numeric_limits<double>::epsilon()) return 0;
  f20 = (vertex(1, 2) - vertex(1, 0)) * vertex(0, 1) +
        (vertex(0, 0) - vertex(0, 2)) * vertex(1, 1) + cp20;
  if (std::fabs(f20) <= std::numeric_limits<double>::epsilon()) return 0;

  double coe = 0.0;
  coe = ((vertex(1, 1) - vertex(1, 2)) * x + (vertex(0, 2) - vertex(0, 1)) * y +
         cp12) /
        f12;
  if (0 > coe) return 0;
  bc[0] = coe;
  coe = (vertex(1, 2) - vertex(1, 0)) * x + (vertex(0, 0) - vertex(0, 2)) * y +
        cp20;
  if (0 > coe) return 0;
  bc[1] = coe;
  coe = 1. - bc[0] - bc[1];
  if (0 > coe) return 0;
  bc[2] = coe;
  return 1;
}

bool depthTest(int x, int y) {
  double depth = vertex(2, 0) * barycentric[0] + vertex(2, 1) * barycentric[1] +
                 vertex(2, 2) * barycentric[2];
  int i = x * buffer->GetWidth() + y;
  if (depth > zbuffer[i]) {
    zbuffer[i] = depth;
    return true;
  } else {
    return false;
  }
}

void draw() {
  std::size_t tri_count = model->triangleCount();
  for (int i = 0; i < tri_count; ++i) {
    auto vertexIds = model->nthTriVertexIndexs(i);
    auto uvIds = model->nthTriUvIndexs(i);
    auto normalIds = model->nthTriNormalIndexs(i);
    for (int n = 0; n < 3; ++n) {
      vertex.replaceNthCol(n, model->nthVertex(vertexIds[n]));
      vertex(3, n) = 1;  // w
      uv.replaceNthCol(n, model->nthUv(uvIds[n]));
      normal.replaceNthCol(n, model->nthNormal(normalIds[n]));
    }
    if (shader) {
      // shader
    } else {
      // modelProcess()
      viewProcess();
      projecionProcess();
      viewportProcess();
    }
    rasterization();
  }
}

void save(const std::string& filename) {
  if (model) delete model;
  if (texture) delete texture;

  buffer->WriteTgaFile(filename, false, false, false);
  delete buffer;
}
