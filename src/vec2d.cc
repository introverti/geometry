/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/src/vec2d.cc
 */
#include "vec2d.h"

#include <stdexcept>

namespace innovusion {
namespace geometry {

Vec2d::Vec2d() : y(0.0), z(0.0) {}

Vec2d::Vec2d(double y_, double z_) : y(y_), z(z_) {}

Vec2d Vec2d::operator-() const { return Vec2d(-y, -z); }

Vec2d Vec2d::operator+(const Vec2d& other) const {
  return Vec2d(y + other.y, z + other.z);
}

Vec2d Vec2d::operator-(const Vec2d& other) const {
  return Vec2d(y - other.y, z - other.z);
}

Vec2d& Vec2d::operator++() {
  ++y;
  ++z;
  return *this;
}

Vec2d& Vec2d::operator--() {
  --y;
  --z;
  return *this;
}

Vec2d Vec2d::operator++(int) {
  Vec2d curr = *this;
  this->operator++();
  return curr;
}

Vec2d Vec2d::operator--(int) {
  Vec2d curr = *this;
  this->operator--();
  return curr;
}

Vec2d Vec2d::operator+(double scalar) const {
  return Vec2d(y + scalar, z + scalar);
}

Vec2d Vec2d::operator-(double scalar) const {
  return Vec2d(y - scalar, z - scalar);
}

double Vec2d::operator*(const Vec2d& other) const {
  return y * other.y + z * other.z;
}

double Vec2d::operator^(const Vec2d& other) const {
  return y * other.z - z * other.y;
}

Vec2d Vec2d::operator*(double coeff) const {
  return Vec2d(y * coeff, z * coeff);
}

Vec2d Vec2d::operator/(double coeff) const {
  if (std::fabs(coeff) < kGeometryEpsilon) {
    throw std::runtime_error(
        "Input coefficient is null, cannot be used as divisor");
  }
  return Vec2d(y / coeff, z / coeff);
}

bool Vec2d::operator==(const Vec2d& other) const {
  return (std::fabs(y - other.y) < kGeometryEpsilon &&
          std::fabs(z - other.z) < kGeometryEpsilon);
}

bool Vec2d::operator!=(const Vec2d& other) const {
  return !(this->operator==(other));
}

Vec2d& Vec2d::operator+=(const Vec2d& other) {
  y += other.y;
  z += other.z;
  return *this;
}

Vec2d& Vec2d::operator-=(const Vec2d& other) {
  y -= other.y;
  z -= other.z;
  return *this;
}

Vec2d& Vec2d::operator*=(double coeff) {
  y *= coeff;
  z *= coeff;
  return *this;
}

Vec2d& Vec2d::operator/=(double coeff) {
  if (std::fabs(coeff) < kGeometryEpsilon) {
    throw std::runtime_error(
        "Input coefficient is null, cannot be used as divisor");
  }
  y /= coeff;
  z /= coeff;
  return *this;
}

bool Vec2d::isZeroVector() const { return this->norm() < kGeometryEpsilon; }

double Vec2d::normSquare() const { return y * y + z * z; }

double Vec2d::norm() const { return std::sqrt(normSquare()); }

Vec2d Vec2d::normalize() {
  const double mod = norm();
  if (mod < kGeometryEpsilon) {
    throw std::runtime_error("Vec2d is null vector, cannot be normalized");
  }
  this->operator/=(mod);
  return *this;
}

double Vec2d::distanceTo(const Vec2d& other) const {
  double dis = std::hypot(y - other.y, z - other.z);
  return dis < kGeometryEpsilon ? 0.0 : dis;
}

double Vec2d::distanceSquareTo(const Vec2d& other) const {
  const double dy = y - other.y;
  const double dz = z - other.z;
  return dy * dy + dz * dz;
}

Vec2d operator*(double coeff, const Vec2d& origin) { return origin * coeff; }

}  // namespace geometry
}  // namespace innovusion
