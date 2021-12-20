#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <array>
#include <cassert>
#include <cmath>
#include <limits>

////////////////////
// vector
template <typename T, int N>
class Vec {
 public:
  Vec() : data_{} {}
  Vec(std::initializer_list<T> l) {
    assert(N == l.size());
    auto begin = l.begin();
    for (int i = 0; i < N; ++i) {
      data_[i] = *(begin + i);
    }
  }
  Vec(const Vec& v) = default;
  Vec& operator=(const Vec& rhs) = default;
  ~Vec() = default;

  T& operator[](int i) {
    assert(i < N);
    return data_[i];
  }

  const T& operator[](int i) const {
    assert(i < N);
    return data_[i];
  }

  double norm() const {
    double norm = 0;
    for (int i = 0; i < N; ++i) {
      norm += data_[i] * data_[i];
    }
    return std::sqrt(norm);
  }

  friend Vec operator*(const Vec& lhs, const double rhs) {
    Vec v = lhs;
    for (int i = 0; i < N; ++i) {
      v.data_[i] *= rhs;
    }
    return v;
  }

  friend Vec operator*(const double lhs, const Vec& rhs) { return rhs * lhs; }

  friend Vec operator/(const Vec& numerator, const double denominator) {
    assert(std::abs(denominator) > std::numeric_limits<double>::epsilon());
    Vec v = numerator;
    for (int i = 0; i < N; ++i) {
      v.data_[i] /= denominator;
    }
    return v;
  }

 private:
  std::array<T, N> data_;
};

template <typename T, int N>
Vec<T, N> operator*(const Vec<T, N>& lhs, const double rhs);

template <typename T, int N>
Vec<T, N> operator*(const double lhs, const Vec<T, N>& rhs);

template <typename T, int N>
Vec<T, N> operator/(const Vec<T, N>& numerator, const double denominator);

using Vec2 = Vec<double, 2>;
using Vec3 = Vec<double, 3>;
using Vec4 = Vec<double, 4>;
using Vec3Int = Vec<int, 3>;

namespace vector {
template <int N>
Vec<double, N> normalize(const Vec<double, N>& v) {
  return v / v.norm();
};

Vec3 cross(const Vec3& v1, const Vec3& v2);
}  // namespace vector

////////////////////
// matrix
template <int Row, int Col>
class Mat {
 public:
  Mat() : data_{} {}
  Mat(const Mat& m) = default;
  Mat& operator=(const Mat& rhs) = default;
  ~Mat() = default;

  double& operator()(int i, int j) {
    assert(i < Row && j < Col);
    return data_[i * Col + j];
  }

  const double& operator()(int i, int j) const {
    assert(i < Row && j < Col);
    return data_[i * Col + j];
  }

  Vec<double, Row> extractNthCol(int n) const {
    assert(n < Col);
    Vec<double, Row> c;
    for (int i = 0; i < Row; ++i) {
      c[i] = data_[i * Col + n];
    }
    return c;
  }

  template <int N>
  void replaceNthCol(int n, const Vec<double, N>& src) {
    assert(n < Col && N <= Row);
    for (int i = 0; i < N; ++i) {
      data_[i * Col + n] = src[i];
    }
  }

  template <int N>
  void replaceNthRow(int n, const Vec<double, N>& src) {
    assert(n < Row && N <= Col);
    for (int j = 0; j < N; ++j) {
      data_[j + n * Col] = src[j];
    }
  }

 private:
  std::array<double, Row * Col> data_;
};

using Mat3 = Mat<3, 3>;
using Mat4 = Mat<4, 4>;

template <int Row, int Col, int Com>
Mat<Row, Col> operator*(const Mat<Row, Com>& m1, const Mat<Com, Col>& m2) {
  Mat<Row, Col> m;
  for (int i = 0; i < Row; ++i) {
    for (int j = 0; j < Col; ++j) {
      double sum = 0;
      for (int k = 0; k < Com; ++k) {
        sum += m1(i, k) * m2(k, j);
      }
      m(i, j) = sum;
    }
  }
  return m;
}

template <int Row, int Com>
Vec<double, Row> operator*(const Mat<Row, Com>& m, const Vec<double, Com>& v) {
  Vec<double, Row> v2;
  for (int i = 0; i < Row; ++i) {
    double sum = 0;
    for (int j = 0; j < Com; ++j) {
      sum += m(i, j) * v[j];
    }
    v2[i] = sum;
  }
  return v2;
}

namespace matrix {
Mat3 genIMat3();
Mat4 genIMat4();
}  // namespace matrix

#endif  // GEOMETRY_H_
