/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-13
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/src/multiple_polygon2d.cc
 */

#include "multiple_polygon2d.h"

namespace innovusion {
namespace geometry {

MultiplePolygon2d::MultiplePolygon2d() : polygons_({}) {}

bool MultiplePolygon2d::add(size_t index, const std::vector<Vec2d>& outer,
                            const std::vector<std::vector<Vec2d>>& inners) {
  PolygonPtr ptr_polygon =
      std::make_shared<Polygon2d>(PolygonType::Polygon, outer, inners);

  if (!ptr_polygon->isValid()) {
    return false;
  }  // Not valid polygon

  const auto& it = polygons_.find(index);

  if (it == polygons_.end()) {
    if (overlaped(ptr_polygon)) {
      return false;
    } else {
      polygons_.emplace(index, ptr_polygon);
      return true;
    }
  } else {
    // if exist we need to remove current first than check for overlaped
    auto temp_ptr = it->second;
    polygons_.erase(it);
    if (overlaped(ptr_polygon)) {
      polygons_[index] = temp_ptr;  // recover
      return false;
    } else {
      polygons_[index] = ptr_polygon;  // replace
      return true;
    }
  }
}

bool MultiplePolygon2d::remove(size_t index) {
  return polygons_.erase(index) == 1;
}

bool MultiplePolygon2d::overlaped(const PolygonPtr& polygon) const {
  return iouTarget(polygon) > 0;
}

int MultiplePolygon2d::iouTarget(const PolygonPtr& others) const {
  return std::accumulate(polygons_.begin(), polygons_.end(), 0,
                         [&](int sum, const auto& ptr) {
                           return sum + ptr.second->iouTarget(others);
                         });
}

bool MultiplePolygon2d::within(const Vec2d& point) const {
  return std::any_of(polygons_.begin(), polygons_.end(), [&](const auto& ptr) {
    return ptr.second->within(point);
  });
}

bool MultiplePolygon2d::covered(const Vec2d& point) const {
  return std::any_of(polygons_.begin(), polygons_.end(), [&](const auto& ptr) {
    return ptr.second->covered(point);
  });
}

bool Roi2d::isUseful(const PolygonPtr& others) const {
  bool result = iouTarget(others) > rate_;
  return intreseted_ ? result : !result;
}

bool Roi2d::isUseful(const Vec2d& point) const {
  return intreseted_ ? within(point) : !within(point);
}

}  // namespace geometry
}  // namespace innovusion
