/***
 * @Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-13
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/include/region_monitor.h
 */

#pragma once
#include <mutex>
#include <unordered_map>
#include <vector>

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "Eigen/LU"
#include "multiple_polygon2d.h"
#include "region2d.h"
#include "vec2d.h"

namespace innovusion {
namespace geometry {
typedef std::shared_ptr<Region2d> RegionPtr;
typedef std::shared_ptr<Box2d> BoxPtr;
typedef std::shared_ptr<Roi2d> RoiPtr;

class RegionException : public std::runtime_error {
 public:
  RegionException(const std::string& msg) : std::runtime_error(msg) {}
  ~RegionException() = default;
};

class RegionMonitor final {
 public:
  RegionMonitor(const std::vector<int32_t>& resolution, bool roi_flag = true,
                int roi_rate = 50);
  ~RegionMonitor() = default;

  /***
   * @remark GUARDED BY MUTEX
   * @description: Add region to region container
   * @param region_id  Key used to distinguish different region
   * @param outer Outer boundary of region polygon
   * @param inners inners holes of region polygon
   * @param attribute attribute (should not exceed resolution's size)
   * @param value value will multiple correspond resolution
   * @return  return ture if successfully add region
   */
  bool add(size_t region_id, const std::vector<Vec2d>& outer,
           const std::vector<std::vector<Vec2d>>& inners,
           const std::vector<uint32_t>& attribute,
           const std::vector<int32_t>& value);

  /***
   * @remark GUARDED BY MUTEX
   * @description: Remove region from container if exist
   * @param region_id region index
   */
  bool remove(size_t region_id);

  // /***
  //  * @remark GUARDED BY MUTEX
  //  * @description: Clear region container
  //  */
  // void clear();

  /***
   * @remark GUARDED BY MUTEX
   * @description: Insert correspond attributes, values and ious of regions who
   * has overlap area with input box
   * @remark Used to reconstruct cluster box
   * @param center_point
   * @param length
   * @param width
   * @param spindle
   *
   * @remark Used to send variables
   * @param attributes
   * @param values
   * @param ious
   */
  void findRelatedMessage(const Vec2d& center_point, float length, float width,
                          uint32_t spindle, std::vector<uint32_t>* attributes,
                          std::vector<int32_t>* values,
                          std::vector<int32_t>* ious,
                          std::unordered_map<uint32_t, uint32_t>* flow);
  void findRelatedMessage(const BoxPtr bbox, std::vector<uint32_t>* attributes,
                          std::vector<int32_t>* values,
                          std::vector<int32_t>* ious,
                          std::unordered_map<uint32_t, uint32_t>* flow);
  /***
   * @remark GUARDED BY MUTEX
   * @description: Get all current valid bkg's outer polygon
   * @return Vector of Polygon2d, 2D vector of Vec2d
   * @remark First Polygon corner is repeated as Polygon needs to be enclosed
   */
  std::vector<std::vector<Vec2d>> getBkg();


  /***
   * @remark DEBUG ONLY WITH LOCK
   * @description: Return target region's shared_ptr
   */
  RegionPtr getRegion(size_t index);
  /***
   * @remark DEBUG ONLY NO LOCK
   * @description: Return the resolution vector of current region-monitor
   */
  std::vector<int32_t> resolution();

  bool isUseful(const PolygonPtr& others);
  bool isUseful(const Vec2d& points);
  bool addRoi(size_t index, const std::vector<Vec2d>& outer,
              const std::vector<std::vector<Vec2d>>& inners);
  bool removeRoi(size_t index);
  bool initRtMatrix(Eigen::Matrix4f matrix, float precision);

  Vec2d doRotatTrans(const Vec2d& source) const;
  std::vector<Vec2d> doRotatTrans(const std::vector<Vec2d>& source) const;
  std::vector<std::vector<Vec2d>> doRotatTrans(
      const std::vector<std::vector<Vec2d>>& source) const;
  int32_t computeHeading(const Eigen::Matrix3f& rotation);
  void computeHeading(int32_t& angle);


  /***
   * @remark DEBUG ONLY NO LOCK
   * @description: Get a copy of current region container
   */
  inline std::unordered_map<size_t, RegionPtr> getRegionMap() {
    return region_;
  }
  inline bool needRt() { return rt_flag_; }
  inline std::vector<Vec2d> getRotation() { return rotation_; }
  inline Vec2d getTranslation() { return translation_; }
  inline int32_t getHeading() { return heading_; }
  inline RoiPtr getRoi() { return roi_; }
  inline bool roiReady() const { return !roi_->empty(); }

 private:
  std::vector<int32_t> resolution_;
  std::unordered_map<size_t, RegionPtr> region_;
  RoiPtr roi_;
  bool rt_flag_;
  std::vector<Vec2d> rotation_;
  Vec2d translation_;
  int32_t heading_;
  std::mutex mutex_;
};

}  // namespace geometry
}  // namespace innovusion
