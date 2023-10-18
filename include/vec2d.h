/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/include/vec2d.h
 */
#pragma once
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometry.hpp>

#include "pre_define.h"

namespace innovusion {
namespace geometry {

constexpr double kGeometryEpsilon = 1e-6;

class Vec2d {
 public:
  Vec2d();
  Vec2d(double y_, double z_);
  virtual ~Vec2d() = default;

  // copy constructor
  Vec2d(const Vec2d& others) = default;
  // copy assignment
  Vec2d& operator=(const Vec2d& other) = default;
  // move constructor
  Vec2d(Vec2d&& other) = default;
  // move assignment
  Vec2d& operator=(Vec2d&& other) = default;

  /***
   * @description: Negative
   */
  NODISCARD Vec2d operator-() const;
  /***
   * @description: Operators
   */
  NODISCARD Vec2d operator+(const Vec2d& other) const;
  NODISCARD Vec2d operator-(const Vec2d& other) const;
  Vec2d& operator++();
  Vec2d& operator--();
  NODISCARD Vec2d operator++(int);
  NODISCARD Vec2d operator--(int);
  NODISCARD Vec2d operator+(double scalar) const;
  NODISCARD Vec2d operator-(double scalar) const;
  NODISCARD double operator*(const Vec2d& other) const;
  NODISCARD double operator^(const Vec2d& other) const;
  NODISCARD Vec2d operator*(double coeff) const;
  NODISCARD Vec2d operator/(double coeff) const;
  NODISCARD bool operator==(const Vec2d& other) const;
  NODISCARD bool operator!=(const Vec2d& other) const;
  Vec2d& operator+=(const Vec2d& other);
  Vec2d& operator-=(const Vec2d& other);
  Vec2d& operator*=(double coeff);
  Vec2d& operator/=(double coeff);

  /***
   * @description: Compare the norm with kGeometryEpsilon
   * @return  True if this Vec2d's norm is null
   */
  bool isZeroVector() const;
  /***
   * @description: Calculate the norm square of Vec2d
   * @return  Norm square value in double type
   */
  double normSquare() const;
  /***
   * @description: Calculate the norm of Vec2d
   * @return  Norm value in double type
   */
  double norm() const;
  /***
   * @description: Normalize this Vec2d
   * @return Normalized Vec2d
   */
  Vec2d normalize();

  /***
   * @description: Calculate the distance between Vec2ds (should we use
   * boost::geometry::distance here ?)
   * @param other another Vec2d
   * @return Distance in type double
   */
  double distanceTo(const Vec2d& other) const;
  /***
   * @description:Calculate the distance square between Vec2ds
   * @param other another Vec2d
   * @return Distance squqre in type double
   */
  double distanceSquareTo(const Vec2d& other) const;
  /***
   * @description: friend operator * ( n * Vec2d)
   */
  friend Vec2d operator*(double coeff, const Vec2d& origin);

 public:
  double y;
  double z;
};
}  // namespace geometry
}  // namespace innovusion

BOOST_GEOMETRY_REGISTER_POINT_2D(innovusion::geometry::Vec2d, double,
                                 cs::cartesian, y, z)
