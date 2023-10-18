/*
 * Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2022-09-13
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/src/region2d.cc
 */
#include "region2d.h"

#include <unordered_map>
#include <vector>

namespace innovusion {
namespace geometry {

Region2d::Region2d(const size_t index, const std::vector<Vec2d>& outer,
                   const std::vector<std::vector<Vec2d>>& inners,
                   const std::vector<uint32_t>& attributes,
                   const std::vector<int32_t>& values)
    : Polygon2d(PolygonType::Region, outer, inners), index_(index) {
  init_ = setAttributesAndValues(attributes, values);
}

bool Region2d::setAttributesAndValues(const std::vector<uint32_t>& attributes,
                                      const std::vector<int32_t>& values) {
  if (attributes.size() != values.size()) {
    return false;
  } else {
    std::unordered_map<uint32_t, int32_t> temp_map = {};
    size_t i = 0;
    size_t limit = attributes.size();
    for (; i < limit; ++i) {
      if (temp_map.find(attributes[i]) == temp_map.end()) {
        temp_map.insert({attributes[i], values[i]});
      } else {
        return false;
      }
    }
    attribute_value_.swap(temp_map);
    return true;
  }
}

void Region2d::getAttributesAndValues(std::vector<uint32_t>* attributes,
                                      std::vector<int32_t>* values) const {
  attributes->clear();
  values->clear();
  attributes->reserve(attribute_value_.size());
  values->reserve(attribute_value_.size());
  for (const auto& pair : attribute_value_) {
    attributes->emplace_back(pair.first);
    values->emplace_back(pair.second);
  }
}

bool Region2d::hasAttribute(const uint32_t& attribute) const {
  return attribute_value_.find(attribute) != attribute_value_.end();
}

bool Region2d::getValue(const uint32_t& attribute, int32_t* value) const {
  const auto it = attribute_value_.find(attribute);
  if (it != attribute_value_.end()) {
    *value = it->second;
    return true;
  } else {
    return false;
  }
}

bool Region2d::isValid() const { return bg::is_valid(polygon_) && init_; }

}  // namespace geometry
}  // namespace innovusion
