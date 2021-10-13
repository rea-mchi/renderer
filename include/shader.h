#ifndef SHADER_H_
#define SHADER_H_

#include <array>
#include <cmath>
#include <string>

#include "include/geometry.h"
#include "include/gl.h"
#include "include/tga_image.h"

class IShader {
 public:
  virtual Vector4 vertex_process(const Vector4& vertex) = 0;
  virtual void fragment_process(const Vector2Int& fragment_coordinates,
                                const Vector3& barycentric) = 0;
  virtual ~IShader() {}
};

class TextureShader : public IShader {
 public:
  TextureShader() = delete;
  explicit TextureShader(const std::string& texture_file);
  TextureShader(const TextureShader& shader) = delete;
  TextureShader& operator=(const TextureShader& rhs) = delete;
  ~TextureShader();
  Vector4 vertex_process(const Vector4& vertex) override;
  void fragment_process(const Vector2Int& fragment_coordinates,
                        const Vector3& barycentric) override;
  void LookAt(const Vector3& camera_pos, const Vector3& gaze_direction,
              const Vector3& viewup);
  // @param near应该为负值，因为相机位于原点向-z轴
  void Projection(const double near);
  void SetViewPort(const double screen_width, const double screen_height);
  // 当前三角形三个顶点的纹理坐标 index [0,2]
  void SetTextureCoordinates(const Vector2& coor, const int index);
  void RegisterCanvas(TgaImage* canvas_ptr);
  void UnregisterCanvas();

 protected:
  SMatrix4 m_camera_;
  SMatrix4 m_proj_;
  SMatrix4 m_vp_;
  TgaImage* canvas_ptr_ = nullptr;

 private:
  std::array<Vector2, 3> texture_coordinates_;
  TgaImage* texture_ptr_ = nullptr;
  double w_ = 0, h_ = 0;
};

TextureShader::TextureShader(const std::string& texture_file)
    : texture_coordinates_() {
  m_camera_ = matrix_m::IMatrix4();
  m_proj_ = matrix_m::IMatrix4();
  m_vp_ = matrix_m::IMatrix4();

  texture_ptr_ = new TgaImage();
  texture_ptr_->ReadTgaFile(texture_file);
  w_ = texture_ptr_->GetWidth();
  h_ = texture_ptr_->GetHeight();
}
TextureShader::~TextureShader() {
  canvas_ptr_ = nullptr;
  delete texture_ptr_;
  texture_ptr_ = nullptr;
}
Vector4 TextureShader::vertex_process(const Vector4& vertex) {
  // printf("x:%f, y:%f, z:%f, w:%f\n", vertex[0], vertex[1], vertex[2],
  //        vertex[3]);
  Vector4 v = m_vp_ * m_proj_ * m_camera_ * vertex;
  // printf("x:%f, y:%f, z:%f, w:%f \n", v[0] / v[3], v[1] / v[3], v[2] / v[3]);
  return v / v[3];
}
void TextureShader::fragment_process(const Vector2Int& fragment_coordinates,
                                     const Vector3& barycentric) {
  // printf("x:%f, y:%f, z:%f\n", barycentric[0], barycentric[1],
  // barycentric[2]);
  double x = vector_m::Dot(barycentric, Vector3{texture_coordinates_[0][0],
                                                texture_coordinates_[1][0],
                                                texture_coordinates_[2][0]});
  double y = vector_m::Dot(barycentric, Vector3{texture_coordinates_[0][1],
                                                texture_coordinates_[1][1],
                                                texture_coordinates_[2][1]});
  canvas_ptr_->SetColor(
      fragment_coordinates[0], fragment_coordinates[1],
      texture_ptr_->GetColor(std::floor(x * w_), std::floor(y * h_)));
}
void TextureShader::LookAt(const Vector3& camera_pos,
                           const Vector3& gaze_direction,
                           const Vector3& viewup) {
  m_camera_ = CameraTransM(camera_pos, gaze_direction, viewup);
}

void TextureShader::Projection(const double near) {
  m_proj_ = ProjectionM(-1, 1, -1, 1, -5, -1, true);
}

void TextureShader::SetViewPort(const double screen_width,
                                const double screen_height) {
  m_vp_ = ViewportTransM(screen_width, screen_height);
}

void TextureShader::SetTextureCoordinates(const Vector2& coor,
                                          const int index) {
  texture_coordinates_[index] = coor;
}

void TextureShader::RegisterCanvas(TgaImage* canvas_ptr) {
  canvas_ptr_ = canvas_ptr;
}

void TextureShader::UnregisterCanvas() { canvas_ptr_ = nullptr; }

#endif  // SHADER_H_
