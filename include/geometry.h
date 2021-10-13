#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <array>
#include <cmath>
#include <iostream>

template <typename T, int N>
class Vector {
 public:
  Vector();
  Vector(std::initializer_list<T> list);
  Vector(const Vector& vector);
  Vector& operator=(const Vector& rhs);
  ~Vector() = default;

  T& operator[](std::size_t n) { return data_[n]; }
  const T& operator[](std::size_t n) const { return data_[n]; }

  double Norm() const;

 private:
  std::array<T, N> data_;
};

using Vector2 = Vector<double, 2>;
using Vector2Int = Vector<int, 2>;
using Vector3 = Vector<double, 3>;
using Vector3Int = Vector<int, 3>;
using Vector4 = Vector<double, 4>;

template <typename T, int N>
Vector<T, N>::Vector() : data_() {
  data_.fill(0);
}

template <typename T, int N>
Vector<T, N>::Vector(std::initializer_list<T> list) : data_() {
  if (N == list.size()) {
    auto it = list.begin();
    for (int i = 0; i < N; ++i, ++it) {
      data_[i] = *it;
    }
  } else {
    data_.fill(0);
  }
}

template <typename T, int N>
Vector<T, N>::Vector(const Vector& vector) : data_() {
  for (int i = 0; i < N; ++i) {
    data_[i] = vector[i];
  }
}

template <typename T, int N>
Vector<T, N>& Vector<T, N>::operator=(const Vector& rhs) {
  for (int i = 0; i < N; ++i) {
    data_[i] = rhs[i];
  }
  return *this;
}

template <typename T, int N>
double Vector<T, N>::Norm() const {
  double norm2 = 0;
  for (int i = 0; i < N; ++i) {
    norm2 += data_[i] * data_[i];
  }
  return std::sqrt(norm2);
}

// arithmetic operator overloading
template <typename T, int N>
Vector<T, N> operator+(const Vector<T, N>& lhs, const T& rhs) {
  Vector<T, N> res = lhs;
  for (int i = 0; i < N; ++i) {
    res[i] += rhs;
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N>& lhs, const T& rhs) {
  Vector<T, N> res = lhs;
  for (int i = 0; i < N; ++i) {
    res[i] -= rhs;
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator*(const Vector<T, N>& lhs, const T& rhs) {
  Vector<T, N> res = lhs;
  for (int i = 0; i < N; ++i) {
    res[i] *= rhs;
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator*(const T& lhs, const Vector<T, N>& rhs) {
  return rhs * lhs;
}
template <typename T, int N>
Vector<T, N> operator/(const Vector<T, N>& lhs, const T& rhs) {
  if (0 == rhs) {
    std::cerr << "Vector can't be divided by 0.\n";
    exit(1);
  }
  Vector<T, N> res = lhs;
  for (int i = 0; i < N; ++i) {
    res[i] /= rhs;
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator+(const Vector<T, N>& lhs, const Vector<T, N>& rhs) {
  Vector<T, N> res = lhs;
  for (int i = 0; i < N; ++i) {
    res[i] += rhs[i];
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N>& lhs, const Vector<T, N>& rhs) {
  Vector<T, N> res = lhs;
  for (int i = 0; i < N; ++i) {
    res[i] -= rhs[i];
  }
  return res;
}

namespace vector_m {
template <int N>
double Dot(const Vector<double, N>& v1, const Vector<double, N>& v2) {
  double sum = 0;
  for (int i = 0; i < N; ++i) {
    sum += v1[i] * v2[i];
  }
  return sum;
}

Vector3 Cross(const Vector2& v1, const Vector2& v2);
Vector3 Cross(const Vector3& v1, const Vector3& v2);
Vector3 Cross(const Vector4& v1, const Vector4& v2);

template <int N>
Vector<double, N> Normalize(const Vector<double, N>& v) {
  double norm = v.Norm();
  if (0 == norm) {
    return v;
  }

  return v / norm;
}

Vector4 HomogeneousCoords(const Vector3& vector3);
}  // namespace vector_m

// matrix
template <typename T, int ROW, int COL>
class Matrix {
 public:
  Matrix();
  Matrix(const Matrix& m);
  Matrix& operator=(const Matrix& rhs);
  ~Matrix() = default;

  T& operator()(int i, int j) { return data_[i * COL + j]; }
  const T& operator()(int i, int j) const { return data_[i * COL + j]; }

 private:
  std::array<T, ROW * COL> data_;
};

using SMatrix3 = Matrix<double, 3, 3>;
using SMatrix4 = Matrix<double, 4, 4>;

template <typename T, int ROW, int COL>
Matrix<T, ROW, COL>::Matrix() : data_() {
  data_.fill(0);
}

template <typename T, int ROW, int COL>
Matrix<T, ROW, COL>::Matrix(const Matrix& m) : data_() {
  for (int i = 0; i < ROW; ++i) {
    for (int j = 0; j < COL; ++j) {
      data_[i * COL + j] = m(i, j);
    }
  }
}

template <typename T, int ROW, int COL>
Matrix<T, ROW, COL>& Matrix<T, ROW, COL>::operator=(const Matrix& rhs) {
  for (int i = 0; i < ROW; ++i) {
    for (int j = 0; j < COL; ++j) {
      data_[i * COL + j] = rhs(i, j);
    }
  }
  return *this;
}

// operator
// m[ROW][COM]*m[COM][COL]
template <typename T, int ROW, int COM, int COL>
Matrix<T, ROW, COL> operator*(const Matrix<T, ROW, COM>& lhs,
                              const Matrix<T, COM, COL>& rhs) {
  Matrix<T, ROW, COL> res;
  for (int i = 0; i < ROW; ++i) {
    for (int j = 0; j < COL; ++j) {
      T value = 0;
      for (int k = 0; k < COM; ++k) {
        value += lhs(i, k) * rhs(k, j);
      }
      res(i, j) = value;
    }
  }
  return res;
}

// matrix*vector->vector
// 返回一定是列向量
template <typename T, int ROW, int COM>
Vector<T, ROW> operator*(const Matrix<T, ROW, COM>& lhs,
                         const Vector<T, COM>& rhs) {
  Vector<T, ROW> res;
  for (int i = 0; i < ROW; ++i) {
    T value = 0;
    for (int j = 0; j < COM; ++j) {
      value += lhs(i, j) * rhs[j];
    }
    res[i] = value;
  }
  return res;
}

namespace matrix_m {
template <typename T, int ROW, int COL>
Matrix<T, COL, ROW> Transpose(const Matrix<T, ROW, COL>& m) {
  Matrix<T, COL, ROW> t;
  for (int i = 0; i < ROW; ++i) {
    for (int j = 0; j < COL; ++j) {
      t(j, i) = m(i, j);
    }
  }
  return t;
}

SMatrix3 IMatrix3();
SMatrix4 IMatrix4();
}  // namespace matrix_m

#endif  // GEOMETRY_H_
