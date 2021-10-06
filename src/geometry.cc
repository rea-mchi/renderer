#include "include/geometry.h"

#include <cmath>
#include <iostream>

// vector
template <typename T, int N>
Vector<T, N>::Vector() : data_() {
  data_.fill(0);
}

template <typename T, int N>
Vector<T, N>::Vector(std::initializer_list<T> list) : data_() {
  if (N == list.size()) {
    std::initializer_list<T>::iterator it = list.begin();
    for (int i = 0; i < N; ++i) {
      data_[i] = *(it + i);
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
Vector<T, N>::~Vector() = default;

template <typename T, int N>
T& Vector<T, N>::operator[](std::size_t n) {
  return data_[n];
}

template <typename T, int N>
const T& Vector<T, N>::operator[](std::size_t n) const {
  return data_[n];
}

template <typename T, int N>
double Vector<T, N>::Norm() const {
  double norm2 = 0;
  for (int i = 0; i < N; ++i) {
    norm2 += data_[i] * data_[i];
  }
  return std::sqrt(norm2);
}

inline int& Vector<int, 2>::x() { return data_[0]; }
inline int& Vector<int, 2>::y() { return data_[1]; }
inline double& Vector<double, 2>::x() { return data_[0]; }
inline double& Vector<double, 2>::y() { return data_[1]; }
inline int& Vector<int, 3>::x() { return data_[0]; }
inline int& Vector<int, 3>::y() { return data_[1]; }
inline int& Vector<int, 3>::z() { return data_[2]; }
inline double& Vector<double, 3>::x() { return data_[0]; }
inline double& Vector<double, 3>::y() { return data_[1]; }
inline double& Vector<double, 3>::z() { return data_[2]; }
inline double& Vector<double, 4>::x() { return data_[0]; }
inline double& Vector<double, 4>::y() { return data_[1]; }
inline double& Vector<double, 4>::z() { return data_[2]; }
inline double& Vector<double, 4>::w() { return data_[3]; }

// operator overloading
template <typename T, int N>
Vector<T, N> operator+(const Vector<T, N>& lhs, const T& rhs) {
  Vector<T, N> res = lhs;
  for (int i; i < N; ++i) {
    res[i] += rhs;
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N>& lhs, const T& rhs) {
  Vector<T, N> res = lhs;
  for (int i; i < N; ++i) {
    res[i] -= rhs;
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator*(const Vector<T, N>& lhs, const T& rhs) {
  Vector<T, N> res = lhs;
  for (int i; i < N; ++i) {
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
  for (int i; i < N; ++i) {
    res[i] /= rhs;
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator+(const Vector<T, N>& lhs, const Vector<T, N>& rhs) {
  Vector<T, N> res = lhs;
  for (int i; i < N; ++i) {
    res[i] += rhs[i];
  }
  return res;
}
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N>& lhs, const Vector<T, N>& rhs) {
  Vector<T, N> res = lhs;
  for (int i; i < N; ++i) {
    res[i] -= rhs[i];
  }
  return res;
}

template <int N>
double vector_m::Dot(Vector<double, N> v1, Vector<double, N> v2) {
  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += v1[i] * v2[i];
  }
  return sum;
}

Vector3 vector_m::Cross(Vector2 v1, Vector2 v2) {
  return Vector3({0, 0, v1[0] * v2[1] - v1[1] * v2[0]});
}

Vector3 vector_m::Cross(Vector3 v1, Vector3 v2) {
  return Vector3({v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2],
                  v1[0] * v2[1] - v1[1] * v2[0]});
}

template <int N>
Vector<double, N> Normalize(Vector<double, N> v) {
  double norm = v.Norm();
  if (0 == norm) {
    return v;
  }

  return v / norm;
}

// explicitly instantiate
template class Vector<double, 2>;
template class Vector<int, 2>;
template class Vector<double, 3>;
template class Vector<int, 3>;
template class Vector<double, 4>;

// matrix
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

template <typename T, int ROW, int COL>
Matrix<T, ROW, COL>::~Matrix() = default;

template <typename T, int ROW, int COL>
T& Matrix<T, ROW, COL>::operator()(int i, int j) {
  return data_[i * COL + j];
}

template <typename T, int ROW, int COL>
Matrix<T, ROW, COL> Matrix<T, ROW, COL>::Transpose() {
  if (ROW != COL) {
    std::cerr << "This matrix can't be transposed! Row: " << ROW
              << " Col: " << COL << std::endl;
    exit(1);
  }
  Matrix t = *this;
  for (int i = 1; i < ROW; ++i) {
    for (int j = 0; j < i; ++j) {
      std::swap(data_[i * COL + j], data_[j * COL + i]);
    }
  }
  return t;
}

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

SMatrix3 matrix_m::IMatrix3() {
  SMatrix3 m;
  for (int i = 0; i < 3; ++i) {
    m(i, i) = 1;
  }
  return m;
}

SMatrix4 matrix_m::IMatrix4() {
  SMatrix4 m;
  for (int i = 0; i < 4; ++i) {
    m(i, i) = 1;
  }
  return m;
}

template class Matrix<double, 3, 3>;
template class Matrix<double, 4, 4>;
