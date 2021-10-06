#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <array>

template <typename T, int N>
class Vector {
 public:
  Vector();
  Vector(std::initializer_list<T> list);
  Vector(const Vector& vector);
  Vector& operator=(const Vector& rhs);
  ~Vector();
  T& operator[](std::size_t n);
  const T& operator[](std::size_t n) const;
  double Norm() const;
  T& x();
  T& y();
  T& z();
  T& w();

 private:
  std::array<T, N> data_;
};

template <typename T, int N>
Vector<T, N> operator+(const Vector<T, N>& lhs, const T& rhs);
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N>& lhs, const T& rhs);
template <typename T, int N>
Vector<T, N> operator*(const Vector<T, N>& lhs, const T& rhs);
template <typename T, int N>
Vector<T, N> operator*(const T& lhs, const Vector<T, N>& rhs);
template <typename T, int N>
Vector<T, N> operator/(const Vector<T, N>& lhs, const T& rhs);
template <typename T, int N>
Vector<T, N> operator+(const Vector<T, N>& lhs, const Vector<T, N>& rhs);
template <typename T, int N>
Vector<T, N> operator-(const Vector<T, N>& lhs, const Vector<T, N>& rhs);

namespace vector_m {
template <int N>
double Dot(Vector<double, N> v1, Vector<double, N> v2);

Vector<double, 3> Cross(Vector<double, 2> v1, Vector<double, 2> v2);
Vector<double, 3> Cross(Vector<double, 3> v1, Vector<double, 3> v2);

template <int N>
Vector<double, N> Normalize(Vector<double, N> v);
}  // namespace vector_m

using Vector2 = Vector<double, 2>;
using Vector2Int = Vector<int, 2>;
using Vector3 = Vector<double, 3>;
using Vector3Int = Vector<int, 3>;
using Vector4 = Vector<double, 4>;

template <typename T, int ROW, int COL>
class Matrix {
 public:
  Matrix();
  Matrix(const Matrix& m);
  Matrix& operator=(const Matrix& rhs);
  ~Matrix();
  T& operator()(int i, int j);
  Matrix Transpose();

 private:
  std::array<T, ROW * COL> data_;
};

// m[ROW][COM]*m[COM][COL]
template <typename T, int ROW, int COM, int COL>
Matrix<T, ROW, COL> operator*(const Matrix<T, ROW, COM>& lhs,
                              const Matrix<T, COM, COL>& rhs);

namespace matrix_m {
Matrix<double, 3, 3> IMatrix3();
Matrix<double, 4, 4> IMatrix4();
}  // namespace matrix_m

using SMatrix3 = Matrix<double, 3, 3>;
using SMatrix4 = Matrix<double, 4, 4>;

#endif  // GEOMETRY_H_
