/*
 * Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-13
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/src/region_monitor.cc
 */
#include "region_monitor.h"

#include <iostream>

namespace innovusion {
namespace geometry {
RegionMonitor::RegionMonitor(const std::vector<int32_t>& resolution,
                             const bool& roi_flag, const int& roi_rate)
    : resolution_(resolution) {
  region_ = {};
  roi_ = std::make_shared<Roi2d>(roi_flag, roi_rate);
  rt_flag_ = false;
  rotation_ = std::vector<Vec2d>(2, Vec2d());
  heading_ = 0;
  translation_ = Vec2d();
}

bool RegionMonitor::add(const size_t& region_id,
                        const std::vector<Vec2d>& outer,
                        const std::vector<std::vector<Vec2d>>& inners,
                        const std::vector<uint32_t>& attribute,
                        const std::vector<int32_t>& value) {
  bool is_roi_region = false;
  std::vector<uint32_t> add_attribute{};
  std::vector<int32_t> add_value{};

  if (attribute.size() != value.size()) {
    throw RegionException(
        "Attribute's element number does not match with "
        "Value's. " +
        std::to_string(attribute.size()) + "!=" + std::to_string(value.size()));
  } else {
    for (size_t i = 0; i < attribute.size(); ++i) {
      if ((attribute[i] - 1) % resolution_.size() == 8) {
        is_roi_region = true;
      } else {
        add_attribute.emplace_back(attribute[i]);
        int32_t temp_value = value[i];
        if ((attribute[i] - 1) % resolution_.size() == 0 && rt_flag_) {
          compute_heading(temp_value);
        }
        add_value.emplace_back(
            temp_value * resolution_[(attribute[i] - 1) % resolution_.size()]);
      }
    }
  }

  if (is_roi_region) {
    if (!add_roi(region_id, outer, inners)) {
      throw RegionException("ROI Region " + std::to_string(region_id) +
                            " is not valid");
    }
  } else {
    if (remove_roi(region_id)) {
      AINFO << "Remove Roi attribute of Region ID :" << region_id;
    } else {
      AINFO << "Region ID : " << region_id << "doesnt have Roi attribute.";
    }
  }

  if (add_attribute.size()) {
    RegionPtr ptr_region;
    if (!rt_flag_) {
      ptr_region = std::make_shared<Region2d>(region_id, outer, inners,
                                              add_attribute, add_value);
    } else {
      std::vector<Vec2d> outer_rt = rotat_trans(outer);
      std::vector<std::vector<Vec2d>> inners_rt = rotat_trans(inners);
      ptr_region = std::make_shared<Region2d>(region_id, outer_rt, inners_rt,
                                              add_attribute, add_value);
    }
    if (ptr_region->is_valid()) {
      std::unique_lock<std::mutex> lock(mutex_);
      if (region_.find(region_id) == region_.end()) {
        region_.insert(std::make_pair(region_id, ptr_region));
        AINFO << "Add new Region. ID: " << region_id;
      } else {
        AINFO << "Prev Region is going to be deleted ID: " << region_id;
        region_[region_id] = ptr_region;
        AINFO << "Region is Replaced. ID: " << region_id;
      }
    } else {
      AWARN << "Input Region not valide. ID: " << region_id;
      return false;
    }
  }
  return true;
}

bool RegionMonitor::remove(const size_t& region_id) {
  std::unique_lock<std::mutex> lock(mutex_);
  bool delete_flag = false;
  if (remove_roi(region_id)) {
    AINFO << "ROI ID " << region_id << " is removed";
    delete_flag = true;
  }
  if (region_.find(region_id) != region_.end()) {
    region_.erase(region_id);
    AINFO << "Region ID " << region_id << " is removed";
    delete_flag = true;
  }
  if (!delete_flag) {
    AWARN << "Region ID" << region_id << "not found";
  }
  return delete_flag;
}

// void RegionMonitor::clear() {
//   std::unique_lock<std::mutex> lock(mutex_);
//   region_.clear();
// }

void RegionMonitor::findRelatedMessage(
    const Vec2d& center_point, const float& length, const float& width,
    const uint32_t& spindle, std::vector<uint32_t>* attributes,
    std::vector<int32_t>* values, std::vector<int32_t>* ious,
    std::unordered_map<uint32_t, uint32_t>* flow) {
  BoxPtr bbox =
      std::make_shared<Box2d>(Box2d(center_point, length, width, spindle));
  findRelatedMessage(bbox, attributes, values, ious, flow);
}

void RegionMonitor::findRelatedMessage(
    const BoxPtr bbox, std::vector<uint32_t>* attributes,
    std::vector<int32_t>* values, std::vector<int32_t>* ious,
    std::unordered_map<uint32_t, uint32_t>* flow) {
  attributes->clear();
  values->clear();
  ious->clear();
  std::unique_lock<std::mutex> lock(mutex_);
  for (auto pair : region_) {
    int rate = pair.second->iou(bbox);
    if (rate > 0) {
      std::vector<uint32_t> temp_attributes;
      std::vector<int32_t> temp_values;

      auto iter = flow->find(pair.first);
      if (iter != flow->end()) {
        ++iter->second;
      } else {
        flow->insert({pair.first, 1});
      }

      pair.second->getAttributesAndValues(&temp_attributes, &temp_values);
      if (temp_attributes.size() == temp_values.size()) {
        std::vector<int32_t> temp_ious(temp_attributes.size(), rate);
        attributes->insert(attributes->end(), temp_attributes.begin(),
                           temp_attributes.end());
        values->insert(values->end(), temp_values.begin(), temp_values.end());
        ious->insert(ious->end(), temp_ious.begin(), temp_ious.end());
      } else {
        AWARN << "Region " << pair.first << "has unmatched Attribute and Value"
              << temp_attributes.size() << "!=" << temp_values.size();
      }
    }
  }
}
RegionPtr RegionMonitor::getRegion(size_t index) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (region_.find(index) != region_.end()) {
    return region_[index];
  } else {
    return nullptr;
  }
}
std::vector<int32_t> RegionMonitor::resolution() { return resolution_; }

std::vector<std::vector<Vec2d>> RegionMonitor::get_bkg() {
  std::vector<std::vector<Vec2d>> bkg{};
  std::unique_lock<std::mutex> lock(mutex_);
  for (auto& pair : region_) {
    bkg.emplace_back(pair.second->get_corners());
  }
  return bkg;
}

bool RegionMonitor::is_useful(const PolygonPtr& others) {
  std::unique_lock<std::mutex> lock(mutex_);
  return roi_->is_useful(others);
}

bool RegionMonitor::is_useful(const Vec2d& points) {
  std::unique_lock<std::mutex> lock(mutex_);
  return roi_->is_useful(points);
}

bool RegionMonitor::add_roi(const size_t index, const std::vector<Vec2d>& outer,
                            const std::vector<std::vector<Vec2d>>& inners) {
  std::unique_lock<std::mutex> lock(mutex_);
  if (rt_flag_) {
    std::vector<Vec2d> outer_rt = rotat_trans(outer);
    std::vector<std::vector<Vec2d>> inners_rt = rotat_trans(inners);
    return roi_->add(index, outer_rt, inners_rt);
  } else {
    return roi_->add(index, outer, inners);
  }
}
bool RegionMonitor::remove_roi(const size_t index) {
  return roi_->remove(index);
}

bool RegionMonitor::init_rt_matrix(Eigen::Matrix4f matrix, float precision) {
  rt_flag_ = false;
  AINFO << matrix;
  Eigen::Matrix3f rotation = matrix.block(0, 0, 3, 3);
  translation_ = {matrix(1, 3), matrix(2, 3)};
  AINFO << "Get Rotation and Transalation matrix"
        << "\n"
        << rotation << "\n"
        << "(" << translation_.y << "," << translation_.z << ")";
  AINFO << "determinant" << rotation.determinant();
  if (std::fabs(rotation.determinant() - 1) < precision) {
    rotation = rotation.inverse().eval();
    AINFO << "Inverse Matrix"
          << "\n"
          << rotation;
  } else {
    AWARN << "Input Rotation matrix is not valid";
    return false;
  }
  if (!rotation.isIdentity() || !translation_.null()) {
    rotation_ = {{rotation(1, 1), rotation(1, 2)},
                 {rotation(2, 1), rotation(2, 2)}};
    heading_ = compute_heading(rotation);
    rt_flag_ = true;
    AINFO << "Enable Rotation and translation";
  } else {
    AWARN << "Input RT matrix is identity";
  }
  return true;
}

Vec2d RegionMonitor::rotat_trans(const Vec2d& source) const {
  Vec2d temp = source - translation_;
  AINFO << "[" << source.y << "," << source.z << "]"
        << " to "
        << "[" << rotation_[0] * temp << "," << rotation_[1] * temp << "]";
  return {rotation_[0] * temp, rotation_[1] * temp};
}

std::vector<Vec2d> RegionMonitor::rotat_trans(
    const std::vector<Vec2d>& source) const {
  std::vector<Vec2d> res{};
  for (const auto& vec : source) {
    res.emplace_back(rotat_trans(vec));
  }
  return res;
}

std::vector<std::vector<Vec2d>> RegionMonitor::rotat_trans(
    const std::vector<std::vector<Vec2d>>& source) const {
  std::vector<std::vector<Vec2d>> res{};
  for (const auto& vecs : source) {
    res.emplace_back(rotat_trans(vecs));
  }
  return res;
}

int32_t RegionMonitor::compute_heading(const Eigen::Matrix3f& rotation) {
  // [-pi:pi]
  // z-y-x
  return -rotation.eulerAngles(2, 1, 0)(2) * 100;
}
void RegionMonitor::compute_heading(int32_t& angle) {
  // covert any angle to [0, 2pi]
  // [-pi :pi] + [0, 2pi] = [-pi:3pi]
  angle += heading_;
  int32_t double_pi = 2 * PI * 100;

  // may not need this
  while (angle < 0) {
    angle += double_pi;
  }
  int turn = angle / double_pi;
  while (turn) {
    angle -= double_pi;
    --turn;
  }
}

}  // namespace geometry
}  // namespace innovusion