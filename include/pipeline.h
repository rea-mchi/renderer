#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <string>

#include "include/geometry.h"
#include "include/model.h"
#include "include/tga_image.h"

void readModel(const std::string& filename);

void readTexture(const std::string& filename);

void initImageBuffer(const int w, const int h, TgaImage::Format format);

void setPointLight(const Vec3& pos, const TgaColor& lightc);

// 右手坐标系。gazeDir表示看向-z方向。
void setCamera(const Vec3& pos, const Vec3& gazeDir, const Vec3& viewupDir);

void setProjection(const double l, const double r, const double b,
                   const double t, const double n, const double f,
                   const bool perspective = true);

void modelProcess();
void viewProcess();
void projecionProcess();
void viewportProcess();
void rasterization();

// @param bc store barycentric only effective when function return 1
// @return 0 if point don't in triangle else 1
int calcBarycentric(const Mat<2, 3>& vertex, const double x, const double y,
                    Vec3& bc);

// @return whether pass test
bool depthTest(int x, int y);

void draw();

void save(const std::string& filename);
#endif  // PIPELINE_H_
