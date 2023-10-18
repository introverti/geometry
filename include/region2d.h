/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-13
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/include/region2d.h
 */
#pragma once
#include <unordered_map>
#include <vector>

#include "polygon2d.h"
#include "vec2d.h"

namespace innovusion {
namespace geometry {

class Region2d final : public Polygon2d {
 public:
  /***
   * @description: Constructor od Region 2D
   */
  Region2d(const size_t index, const std::vector<Vec2d>& outer,
           const std::vector<std::vector<Vec2d>>& inners,
           const std::vector<uint32_t>& attributes,
           const std::vector<int32_t>& values);
  ~Region2d() = default;

  /***
   * @description: Set region's attributes and values, only use for constructor
   * @param attributes attributes should not exceed resolution's size
   * @param values values are already multipied by resolution
   * @return return true if successfully set attributes and values
   */
  NODISCARD bool setAttributesAndValues(const std::vector<uint32_t>& attributes,
                                        const std::vector<int32_t>& values);
  /***
   * @description: Fill region's attributes and values to target containers
   */
  void getAttributesAndValues(std::vector<uint32_t>* attributes,
                              std::vector<int32_t>* values) const;
  /***
   * @description: Find if current region has target attribute
   * @param attribute target attribute
   * @return return true if region has target attribute
   */
  NODISCARD bool hasAttribute(const uint32_t& attribute) const;
  /***
   * @description: Find target attribut's corresponding value
   * @param attribute target attribute
   * @param value target
   * @return true if find target attribute and get correspond value
   */
  NODISCARD bool getValue(const uint32_t& attribute, int32_t* value) const;

  NODISCARD bool isValid() const override;

 private:
  bool init_;
  size_t index_;
  std::unordered_map<uint32_t, int32_t> attribute_value_;
};

class Box2d final : public Polygon2d {
 public:
  /***
   * @description: Call Polygon2d's constructor to init Box2d
   */
  Box2d(const Vec2d& center, const double& length, const double& width,
        const uint32_t& spindle)
      : Polygon2d(PolygonType::Box, center, length, width, spindle) {}
  ~Box2d() = default;
};

}  // namespace geometry
}  // namespace innovusion
