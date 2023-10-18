/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/include/multiple_polygon2d.h
 */
#pragma once
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/geometry/geometry.hpp>
#include <unordered_map>

#include "polygon2d.h"
#include "vec2d.h"
namespace innovusion {
namespace geometry {

namespace bg = boost::geometry;

class MultiplePolygon2d {
 public:
  MultiplePolygon2d();
  virtual ~MultiplePolygon2d() = default;
  NODISCARD bool add(size_t index, const std::vector<Vec2d>& outer,
                     const std::vector<std::vector<Vec2d>>& inners);
  NODISCARD bool remove(size_t index);
  NODISCARD int iouTarget(const PolygonPtr& others) const;
  NODISCARD bool within(const Vec2d& point) const;
  NODISCARD bool covered(const Vec2d& point) const;
  NODISCARD bool overlaped(const PolygonPtr& polygon) const;
  NODISCARD inline bool empty() const { return polygons_.empty(); }

 protected:
  std::unordered_map<size_t, PolygonPtr> polygons_;
};

class Roi2d final : public MultiplePolygon2d {
 public:
  explicit Roi2d(bool intreseted = true, int rate = 50)
      : MultiplePolygon2d(), intreseted_(intreseted), rate_(rate) {}
  ~Roi2d() = default;
  NODISCARD bool isUseful(const PolygonPtr& others) const;
  NODISCARD bool isUseful(const Vec2d& point) const;

 private:
  bool intreseted_;
  int rate_;
};

}  // namespace geometry
}  // namespace innovusion
