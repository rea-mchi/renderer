#ifndef TGA_IMAGE_H_
#define TGA_IMAGE_H_

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// the 18-byte header storing packed RGB data in a 24-bit TGA file
// 设置1个字节对齐，方便读写
#pragma pack(push, 1)
struct TgaHeader {
  std::uint8_t id_field_length;
  // Color map type. 是否存在颜色表
  std::uint8_t color_map_type;
  // 图像数据类型，7种
  std::uint8_t image_data_type;
  // 三个和颜色表相关的数据区
  // 颜色表首地址
  std::uint16_t color_map_entry_origin;
  std::uint16_t color_map_entry_num;
  // 颜色表表项大小， 15, 16, 24, 或者32位
  std::uint8_t color_map_entry_size;
  // 图像数据区
  // 左下角的水平&垂直坐标
  std::uint16_t x_origin;
  std::uint16_t y_origin;
  // 宽高单位均为像素
  std::uint16_t image_width;
  std::uint16_t image_height;
  // 每个像素占用的位数， 8, 16, 24, 32位
  // 8位： 灰度图 - 24位： RGB - 32位： RGBA
  std::uint8_t pixel_depth;
  std::uint8_t image_descriptor;
};
#pragma pack(pop)

// pixel color
// 顺序bgra是因为TGA是little-endian format，而我们写入像素颜色是通过memcpy
struct TgaColor {
  std::int8_t b;
  std::int8_t g;
  std::int8_t r;
  std::int8_t a;

  TgaColor() = default;
  TgaColor(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
      : b(b), g(g), r(r), a(a) {}
  TgaColor(const TgaColor& color) = default;
};

class TgaImage {
 public:
  std::vector<std::uint8_t> data_;
  int width_;
  int height_;
  int bytespp_;

  enum Format {
    kGrayscale = 1,
    kRGB = 3,
    kRGBA = 4,
  };

  TgaImage();
  TgaImage(int width, int height, int bytespp);
  TgaImage(const TgaImage& image);
  ~TgaImage();
  bool ReadTgaFile(const std::string& filename);
  bool WriteTgaFile(const std::string& filename, bool horizontal_flip,
                    bool vertical_flip, bool rle) const;
  bool ReadRleImageData(const std::ifstream& in);
  bool FlipHorizontally();
  bool FlipVertically();
  void SetColor(int x, int y, const TgaColor& color);
};

#endif  // TGA_IMAGE_H_
