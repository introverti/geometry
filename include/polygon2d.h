/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/include/polygon2d.h
 */

#pragma once
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/geometry/geometry.hpp>
#include <memory>
#include <string>
#include <vector>

#include "vec2d.h"
namespace innovusion {
namespace geometry {

namespace bg = boost::geometry;
typedef bg::model::polygon<Vec2d> GPolygon;
typedef bg::model::ring<Vec2d> GRing;
typedef bg::model::box<Vec2d> GBox;
typedef bg::model::segment<Vec2d> Gsegment;

enum class PolygonType {
  Box = 1,
  Region = 2,
  Polygon = 3,
};

class Polygon2d;
typedef std::shared_ptr<Polygon2d> PolygonPtr;

class Polygon2d {
 public:
  /***
   * @description:  Constructor for Box type polygon
   * @param type    Polygon type
   * @param center  Center Point of Box
   * @param length  length of Box
   * @param width   width of Box
   * @param spindle in 0.01 degree, 0 for z axis, clockwise
   */
  Polygon2d(PolygonType type, const Vec2d& center, double length, double width,
            uint32_t spindle);

  /***
   * @description:  Constructor for Region type polygon
   * @param type    Polygon type
   * @param outer   Outside boundary of polygon
   * @param inners  Gather of holes inside polygon
   */
  Polygon2d(PolygonType type, const std::vector<Vec2d>& outer,
            const std::vector<std::vector<Vec2d>>& inners);

  /***
   * @description: Defaut deconstructor
   */
  virtual ~Polygon2d() = default;

  /***
   * @description: Set polyon outer ring
   * @param outer  Outer ring
   */
  void setOuter(const std::vector<Vec2d>& outer);

  /***
   * @description: Set polygon inner rings
   * @param inners Inner rings
   */
  void setInners(const std::vector<std::vector<Vec2d>>& inners);

  /***
   * @description: Corrects a polygon: all rings which are wrongly oriented with
   * respect to their expected orientation are reversed. To all rings which do
   * not have a closing point and are typed as they should have one, the first
   * point is appended.
   */
  void correct();

  /***
   * @description: Checks if a geometry is valid (in the OGC sense)
   */
  NODISCARD virtual bool isValid() const;

  /***
   * @description: Calculates the area of a polygon
   */
  NODISCARD double area() const;

  /***
   * @description: Return a copy of polygon
   */
  NODISCARD GPolygon getPolygon() const;

  /***
   * @description: Calculate the intersection area between two polygons
   * polygon area
   * @param others another polygon
   * @return intersection area
   */
  NODISCARD double calculateIntersectionArea(const PolygonPtr& polygon) const;

  /***
   * @description: Calculate the Intersection area over Union area between two
   * polygons
   * @param others another polygon
   * @return intersection area
   */
  NODISCARD double iou(const PolygonPtr& polygon) const;

  /***
   * @description: Calculate the percentage of intersection area and target
   * polygon area
   * @param others another polygon
   * @return percentage * 100
   */
  NODISCARD int iouTarget(const PolygonPtr& others) const;

  /***
   * @description: Calculate the percentage of intersection area and polygon's
   * own area
   * @param others another polygon
   * @return percentage * 100
   */
  NODISCARD int iouSelf(const PolygonPtr& others) const;

  /***
   * @description: Checks if the point is completely inside self polygon.
   * @param others a point
   * @return true for complete inside
   */
  NODISCARD bool within(const Vec2d& point) const;

  /***
   * @description: Checks if the point is inside or on border self polygon.
   * @param others a point
   * @return true for inside or on border inside
   */
  NODISCARD bool covered(const Vec2d& point) const;

  /***
   * @description: Return the polygon's type
   */
  NODISCARD PolygonType type() const;

  /***
   * @description: Print polygon information as debug string
   */
  NODISCARD std::string debugString() const;

  /***
   * @description: Print polygon information as string
   */
  friend std::ostream& operator<<(std::ostream& os,
                                            const Polygon2d& polygon);

  // (TODO: Tianyun Xuan) remove the following functions as temporary solution
  NODISCARD inline std::vector<Vec2d> f_line() {
    return {f_segment_.first, f_segment_.second};
  }
  NODISCARD inline std::vector<Vec2d> b_line() {
    return {b_segment_.first, b_segment_.second};
  }
  NODISCARD Gsegment f_segment() const { return f_segment_; }
  NODISCARD Gsegment b_segment() const { return b_segment_; }

  NODISCARD double front_distance_to(const Gsegment others) const {
    return bg::distance(f_segment_, others);
  }
  NODISCARD double back_distance_to(const Gsegment others) const {
    return bg::distance(b_segment_, others);
  }

 protected:
  PolygonType type_;
  GPolygon polygon_;
  // (TODO: Tianyun Xuan) remove these variables as temporary solution
  Gsegment f_segment_;
  Gsegment b_segment_;
};

}  // namespace geometry
}  // namespace innovusion
