#include "include/tga_image.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

TgaImage::TgaImage() : width_(0), height_(0), bytespp_(0) {}
TgaImage::TgaImage(int width, int height, int bytespp)
    : data_(width * height * bytespp, 0),
      width_(width),
      height_(height),
      bytespp_(bytespp) {}
TgaImage::TgaImage(const TgaImage& image) = default;
TgaImage::~TgaImage() = default;

int TgaImage::GetWidth() const { return width_; }

int TgaImage::GetHeight() const { return height_; }

int TgaImage::GetBytespp() const { return bytespp_; }

bool TgaImage::ReadTgaFile(const std::string& filename) {
  std::ifstream in;
  const char* cfilename = filename.c_str();
  in.open(cfilename, std::ios::in | std::ios::binary);
  if (!in.is_open()) {
    in.close();
    std::cerr << "Can't open file " << filename << ".\n";
    return false;
  }
  TgaHeader header;
  in.read(reinterpret_cast<char*>(&header), sizeof(header));
  if (!in.good()) {
    in.close();
    std::cerr << "An error occurred while reading header of file.\n";
    return false;
  }
  width_ = static_cast<int>(header.image_width);
  height_ = static_cast<int>(header.image_height);
  bytespp_ = static_cast<int>(header.bits_per_pixel >> 3);
  if (width_ <= 0) {
    in.close();
    std::cerr << "Non-positive value of width: " << width_ << ".\n";
    return false;
  }
  if (height_ <= 0) {
    in.close();
    std::cerr << "Non-positive value of height: " << height_ << ".\n";
    return false;
  }
  if (bytespp_ != kGrayscale && bytespp_ != kRGB && bytespp_ != kRGBA) {
    in.close();
    std::cerr << "Unknown format. Bytes per pixel is: " << bytespp_ << ".\n";
    return false;
  }
  std::size_t bytes_num = width_ * height_ * bytespp_;
  data_ = std::vector<std::uint8_t>(bytes_num, 0);
  int image_type = static_cast<int>(header.image_data_type);
  if (3 == image_type || 2 == image_type) {
    in.read(reinterpret_cast<char*>(data_.data()), bytes_num);
    if (!in.good()) {
      in.close();
      std::cerr << "An error occurred while reading image data in file.\n";
      return false;
    }
  } else if (10 == image_type || 11 == image_type) {
    std::cerr << "The file uses RLE compression.\n";
    DecompressRLE(in, bytes_num, bytespp_);
  } else {
    in.close();
    if (1 == image_type || 9 == image_type) {
      std::cerr << "The data is color-mapped image.\n";
    } else if (0 == image_type) {
      std::cerr << "No image data included.\n";
    } else {
      std::cerr << "Unknown image type.\n";
    }
    return false;
  }
  if (header.image_descriptor & (1 << 4)) FlipHorizontally();
  if (header.image_descriptor & (1 << 5)) FlipVertically();
  in.close();
  std::cerr << width_ << "x" << height_ << ", bytespp: " << bytespp_ << ".\n";
  return true;
}

// ???????????????tga??????????????????????????????????????????????????????????????????????????????
// ???????????????tga????????????header, data, dev area, extension area, footer
// header????????????????????????????????????????????????????????????imagedescriptor??????????????????????????????????????????
// ?????????????????????????????????????????????????????????4-7??????????????????????????????0,?????????????????????
// ??????????????????????????????????????????????????????????????????????????????0????????????????????????
// ????????????26 bytes??? ??????????????????TGA????????????????????????????????????
// ?????????8-25????????????????????????{TRUEVISION-XFILE.\0}??????????????????????????????
// 0-3??????????????????????????? | 4-7???????????????????????????????????? |
// 8-25????????? ???????????????  {TRUEVISION-XFILE.\0}
// @param vertical_flip ??????????????????
// @param horizontal_flip ??????????????????
// @param rle ??????????????????????????????
bool TgaImage::WriteTgaFile(const std::string& filename, bool horizontal_flip,
                            bool vertical_flip, bool rle) const {
  std::uint32_t extension_area_offset = 0;
  std::uint32_t dev_area_offset = 0;
  std::string footer_signature("TRUEVISION-XFILE.");
  std::ofstream out;
  out.open(filename.c_str(), std::ios::trunc | std::ios::binary);
  if (!out.is_open()) {
    out.close();
    std::cerr << "Can't open file " << filename << ".\n";
    return false;
  }
  TgaHeader header;
  header.image_data_type =
      (bytespp_ == kGrayscale ? (rle ? 11 : 3) : (rle ? 10 : 2));
  header.image_width = width_;
  header.image_height = height_;
  header.bits_per_pixel = bytespp_ << 3;
  std::uint8_t descriptor = 0;
  if (horizontal_flip) descriptor |= (1 << 4);
  if (vertical_flip) descriptor |= (1 << 5);
  header.image_descriptor = descriptor;
  out.write(reinterpret_cast<char*>(&header), sizeof(header));
  if (!out.good()) {
    out.close();
    std::cerr << "An error occurred while writing header in file.\n";
    return false;
  }
  if (!rle) {
    out.write(reinterpret_cast<const char*>(data_.data()),
              width_ * height_ * bytespp_);
    if (!out.good()) {
      out.close();
      std::cerr << "An error occurred while writing image data in file.\n";
      return false;
    }
  } else {
    // rle
  }
  out.write(reinterpret_cast<char*>(&extension_area_offset),
            sizeof(extension_area_offset));
  if (!out.good()) {
    out.close();
    std::cerr
        << "An error occurred while writing extension area offset in file.\n";
    return false;
  }
  out.write(reinterpret_cast<char*>(&dev_area_offset), sizeof(dev_area_offset));
  if (!out.good()) {
    out.close();
    std::cerr
        << "An error occurred while writing developer area offset in file.\n";
    return false;
  }
  out.write(footer_signature.c_str(), sizeof(footer_signature));
  if (!out.good()) {
    out.close();
    std::cerr << "An error occurred while writing signature in file.\n";
    return false;
  }
  out.close();
  return true;
}

bool TgaImage::DecompressRLE(std::ifstream& in, const std::size_t bytes_num,
                             const int bytespp) {
  int cur_byte_loc = 0;
  char temp;
  std::uint32_t pixel;
  while (cur_byte_loc < bytes_num) {
    in.read(&temp, 1);
    if (!in.good()) {
      std::cerr << "An error occurred while decompressing RLE (reading packet "
                   "mode).\n";
      return false;
    }
    int n = (temp & 0x7F) + 1;  // 0x7F = 01111111 = 127
    if (temp & 0x80) {
      in.read(reinterpret_cast<char*>(&pixel), bytespp);
      if (!in.good()) {
        std::cerr << "An error occurred while decompressing RLE (reading "
                     "compressed pixel packet).\n";
        return false;
      }
      for (int i = 0; i < n; ++i) {
        std::memcpy(&data_[cur_byte_loc], &pixel, bytespp);
        cur_byte_loc += bytespp;
      }
    } else {
      in.read(reinterpret_cast<char*>(&data_[cur_byte_loc]), n * bytespp);
      cur_byte_loc += n * bytespp;
      if (!in.good()) {
        std::cerr << "An error occurred while decompressing RLE (reading "
                     "uncompressed pixel packet).\n";
        return false;
      }
    }
  }
  return true;
}

bool TgaImage::FlipHorizontally() {
  std::cerr << "flip horizontally.\n";
  return true;
}
bool TgaImage::FlipVertically() {
  std::cerr << "flip vertically.\n";
  return true;
}

void TgaImage::SetColor(int x, int y, const TgaColor& color) {
  if (data_.empty() || x < 0 || y < 0 || x >= width_ || y >= height_) return;
  std::memcpy(data_.data() + (x + y * width_) * bytespp_, &color, bytespp_);
}

TgaColor TgaImage::GetColor(int x, int y) const {
  if (data_.empty() || x < 0 || y < 0 || x >= width_ || y >= height_)
    return TgaColor(255, 255, 255, 255);
  TgaColor color;
  for (int i = 0; i < bytespp_; ++i) {
    color[i] = data_[(x + y * width_) * bytespp_ + i];
  }
  return color;
}
