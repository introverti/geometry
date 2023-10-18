/*
 * Copyright [2022] <Innovusion Inc.>
 * @LastEditTime: 2022-04-09 13:59:13
 * @LastEditors: Tianyun Xuan
 * @FilePath: /apollo/modules/omnisense/region/polygon2d.cc
 */
#include "polygon2d.h"

#include <boost/geometry/algorithms/assign.hpp>
#include <boost/geometry/geometry.hpp>
#include <iostream>

namespace innovusion {
namespace geometry {

namespace bg = boost::geometry;

Polygon2d::Polygon2d(PolygonType type, const Vec2d& center, double length,
                     double width, uint32_t spindle)
    : type_(type) {
  const float value_cos = cos(spindle * M_PI / 18000);
  const float value_sin = sin(spindle * M_PI / 18000);
  const float cos_y = value_cos * width / 2;
  const float cos_z = value_cos * length / 2;
  const float sin_y = value_sin * width / 2;
  const float sin_z = value_sin * length / 2;
  std::vector<Vec2d> bbox = {{cos_y + sin_z, -sin_y + cos_z},
                             {cos_y + (-sin_z), -sin_y + (-cos_z)},
                             {-cos_y + (-sin_z), sin_y + (-cos_z)},
                             {-cos_y + sin_z, sin_y + cos_z}};

  // use transform to replace loop
  std::transform(bbox.begin(), bbox.end(), bbox.begin(),
                 [&center](const Vec2d& point) { return point + center; });

  f_segment_ = {bbox[3], bbox[0]};
  b_segment_ = {bbox[1], bbox[2]};
  setOuter(bbox);
  correct();
}

Polygon2d::Polygon2d(PolygonType type, const std::vector<Vec2d>& outer,
                     const std::vector<std::vector<Vec2d>>& inners)
    : type_(type) {
  setOuter(outer);
  setInners(inners);
  correct();
}

void Polygon2d::setOuter(const std::vector<Vec2d>& outer) {
  bg::assign_points(polygon_, outer);
}

void Polygon2d::setInners(const std::vector<std::vector<Vec2d>>& inners) {
  polygon_.inners().resize(inners.size());
  auto inner = polygon_.inners().begin();
  for (size_t i = 0; i < inners.size(); ++i, ++inner) {
    bg::assign_points(*inner, inners[i]);
  }
}

void Polygon2d::correct() { bg::correct(polygon_); }

bool Polygon2d::isValid() const { return bg::is_valid(polygon_); }

double Polygon2d::area() const { return bg::area(polygon_); }

GPolygon Polygon2d::getPolygon() const { return polygon_; }

double Polygon2d::calculateIntersectionArea(const PolygonPtr& polygon) const {
  std::vector<GPolygon> intersections{};
  bg::intersection(polygon_, polygon->getPolygon(), intersections);
  double sum_area = 0.0;
  for (auto& poly : intersections) {
    sum_area += bg::area(poly);
  }
  return sum_area;
}

double Polygon2d::iou(const PolygonPtr& polygon) const {
  double intersection_area = calculateIntersectionArea(polygon);
  if (intersection_area > 0) {
    return intersection_area / (area() + polygon->area() - intersection_area);
  } else {
    return 0;
  }
}

int Polygon2d::iouTarget(const PolygonPtr& others) const {
  double intersection_area = calculateIntersectionArea(others);
  if (intersection_area > 0) {
    return std::round((intersection_area / others->area()) * 100);
  } else {
    return 0;
  }
}

int Polygon2d::iouSelf(const PolygonPtr& others) const {
  double intersection_area = calculateIntersectionArea(others);
  if (intersection_area > 0) {
    return std::round((intersection_area / area()) * 100);
  } else {
    return 0;
  }
}

bool Polygon2d::within(const Vec2d& point) const {
  return bg::within(point, polygon_);
}

bool Polygon2d::covered(const Vec2d& point) const {
  return bg::covered_by(point, polygon_);
}

PolygonType Polygon2d::type() const { return type_; }

std::string Polygon2d::debugString() const {
  std::ostringstream oss;
  oss << bg::dsv(polygon_);
  return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Polygon2d& polygon) {
  os << polygon.debugString();
  return os;
}

}  // namespace geometry
}  // namespace innovusion
