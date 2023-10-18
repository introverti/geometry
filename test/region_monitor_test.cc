/*
 * Copyright [2021] <Innovusion Inc.>
 * @LastEditTime: 2022-03-30 18:36:37
 * @LastEditors: Tianyun Xuan
 * @FilePath: /omnisense/region/region_monitor_test.cc
 */
#include "region_monitor.h"

#include "test_case.h"
using innovusion::geometry::RegionMonitor;

TEST_F(PolygonTest, Construct) {
  RegionMonitor test_monitor({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  EXPECT_FALSE(test_monitor.roi_ready());
  test_monitor.add(1, rectangle, {}, {9}, {1});
  EXPECT_TRUE(test_monitor.roi_ready());
  test_monitor.add(2, groove, {}, {1}, {1});
  test_monitor.add(3, valley, {}, {2, 3}, {2, 3});
  auto map = test_monitor.get_region_map();
  auto roi = test_monitor.get_roi();
  int32_t tvalue;
  map[2]->get_value(1, &tvalue);
  EXPECT_EQ(tvalue, 1);
  map[3]->get_value(2, &tvalue);
  EXPECT_EQ(tvalue, 4);
  map[3]->get_value(3, &tvalue);
  EXPECT_EQ(tvalue, 9);
  std::shared_ptr<Polygon2d> p_groove = std::make_shared<Polygon2d>(
      Polygon2d(innovusion::geometry::Polygon, groove, {}));
  std::shared_ptr<Polygon2d> p_valley = std::make_shared<Polygon2d>(
      Polygon2d(innovusion::geometry::Polygon, valley, {}));
  auto should_be_groove = map[2];
  EXPECT_EQ(should_be_groove->iou(p_groove), 100);
  EXPECT_EQ(should_be_groove->self_iou(p_groove), 100);
  auto should_be_valley = map[3];
  EXPECT_EQ(should_be_valley->iou(p_valley), 100);
  EXPECT_EQ(should_be_valley->self_iou(p_valley), 100);

  EXPECT_TRUE(map.find(1) == map.end());
  std::shared_ptr<Polygon2d> p_rectangle = std::make_shared<Polygon2d>(
      Polygon2d(innovusion::geometry::Polygon, rectangle, {}));
  EXPECT_EQ(roi->iou(p_rectangle), 100);
}

TEST(Region_monitor, RTmatrix) {
  RegionMonitor test_monitor({});
  float precision = 1e-3;
  Eigen::Matrix4f matrix_one = Eigen::Matrix4f::Identity();
  EXPECT_TRUE(test_monitor.init_rt_matrix(matrix_one, precision));
  EXPECT_FALSE(test_monitor.need_rt());

  Eigen::Matrix4f matrix_two;
  matrix_two << 1, 0, 0, 0, 0, 0.5, float(sqrt(3) / 2), 0, 0,
      float(-sqrt(3) / 2), 0.5, 0, 0, 0, 0, 1;
  EXPECT_TRUE(test_monitor.init_rt_matrix(matrix_two, precision));
  EXPECT_TRUE(test_monitor.need_rt());
  same_point(test_monitor.get_rotation()[0], {0.5, float(-sqrt(3) / 2)});
  same_point(test_monitor.get_rotation()[1], {float(sqrt(3) / 2), 0.5});

  Eigen::Matrix4f matrix_three = Eigen::Matrix4f::Identity();
  EXPECT_TRUE(test_monitor.init_rt_matrix(matrix_three, precision));
  EXPECT_FALSE(test_monitor.need_rt());
  matrix_three(1, 3) = 10;
  EXPECT_TRUE(test_monitor.init_rt_matrix(matrix_three, precision));
  EXPECT_TRUE(test_monitor.need_rt());
  same_point(test_monitor.get_translation(), {10.0, 0.0});
  EXPECT_EQ(test_monitor.get_heading(), 0);

  Vec2d input_vec(10.0, 20.0);
  Vec2d output_vec = test_monitor.rotat_trans(input_vec);
  same_point(output_vec, {0.0, 20.0});

  float angle = std::rand();
  Eigen::Matrix4f matrix_four;
  matrix_four << 1, 0, 0, 0, 0, cosf(angle), sinf(angle), angle, 0,
      -sinf(angle), cosf(angle), -angle, 0, 0, 0, 1;
  EXPECT_TRUE(test_monitor.init_rt_matrix(matrix_four, precision));
  EXPECT_TRUE(test_monitor.need_rt());
  same_point(test_monitor.get_rotation()[0], {cosf(angle), -sinf(angle)});
  same_point(test_monitor.get_rotation()[1], {sinf(angle), cosf(angle)});
  same_point(test_monitor.get_translation(), {angle, -angle});
  Vec2d random_output = test_monitor.rotat_trans({1.0, 1.0});
  Vec2d flat = Vec2d(1.0, 1.0) - Vec2d(angle, -angle);
  same_point({flat * Vec2d(cosf(angle), -sinf(angle)),
              flat * Vec2d(sinf(angle), cosf(angle))},
             random_output);

  // Wuyue lidar_2
  Eigen::Matrix4f matrix_wuyue;
  matrix_wuyue << 0.999938916, 0.00997443964, -0.00476180276, 0.0336134475,
      0.00972163248, -0.998679324, -0.0504489598, 14.5929322, -0.00525871406,
      0.0503995856, -0.998715289, 61.2275867, 0.0, 0.0, 0.0, 1.0;
  EXPECT_TRUE(test_monitor.init_rt_matrix(matrix_wuyue, precision));
  EXPECT_TRUE(test_monitor.need_rt());

  // shandong lidar_2
  Eigen::Matrix4f matrix_shandong;
  matrix_shandong << 0.99954366, 0.00000000, 0.00000000, -0.22692365,
      0.00000144, -0.99397712, 0.10956895, 3.64066433, 0.00000950, -0.10956215,
      -0.99397592, 50.06486676, 0.00000000, 0.00000000, 0.00000000, 1.00000000;
  EXPECT_TRUE(test_monitor.init_rt_matrix(matrix_shandong, precision));
  EXPECT_TRUE(test_monitor.need_rt());
  EXPECT_EQ(test_monitor.get_heading(), -303);
  int32_t heading_origin_one = 297;
  int32_t heading_origin_two = 611;
  test_monitor.compute_heading(heading_origin_one);
  test_monitor.compute_heading(heading_origin_two);
  EXPECT_EQ(heading_origin_one, 622);
  EXPECT_EQ(heading_origin_two, 308);

  //
  Eigen::Matrix4f matrix_reverse;
  matrix_reverse << 1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1;
  EXPECT_TRUE(test_monitor.init_rt_matrix(matrix_reverse, precision));
  EXPECT_TRUE(test_monitor.need_rt());
  EXPECT_EQ(test_monitor.get_heading(), 157);
}