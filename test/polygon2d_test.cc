/*
 * Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2022-09-13
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/test/polygon2d_test.cc
 */

#include "polygon2d.h"

#include <gtest/gtest.h>

#include "region2d.h"

using innovusion::geometry::Box2d;
using innovusion::geometry::Polygon2d;
using innovusion::geometry::PolygonPtr;
using innovusion::geometry::PolygonType;
using innovusion::geometry::Region2d;
using innovusion::geometry::Vec2d;

#define GEOM_EQ(val_1, val_2)        \
  do {                               \
    EXPECT_NEAR(val_1, val_2, 1e-6); \
  } while (0)

void GPOINT_EQ(Vec2d p1, Vec2d p2) {
  GEOM_EQ(p1.y, p2.y);
  GEOM_EQ(p1.z, p2.z);
}

class PolygonTest : public ::testing::Test {
 protected:
  void SetUp() override {
    rectangle = {{-100, -100}, {-100, 100}, {100, 100}, {100, -100}};
    groove = {{-100, -100}, {-100, 100}, {0, 100},   {0, 0},
              {100, 0},     {100, 100},  {200, 100}, {200, -100}};
    // first reversed point cross edges right on corner point
    valley = {{-50, -50}, {-50, 50}, {0, 0}, {50, 50}, {50, -50}};
    // can find first reversed point while prev-flag is the true reversed point
    reversed_l = {{100, 0},    {100, 100}, {200, 100},
                  {200, -100}, {0, -100},  {0, 0}};
    reversed_c = {{0, 100}, {100, 100}, {100, 200}, {0, 200},
                  {0, 300}, {300, 300}, {300, 0},   {0, 0}};
  }
  std::vector<Vec2d> rectangle;
  std::vector<Vec2d> groove;
  std::vector<Vec2d> valley;
  std::vector<Vec2d> reversed_l;
  std::vector<Vec2d> reversed_c;

 public:
  std::vector<Vec2d> drawCircle(Vec2d center, double radius, int num_points) {
    std::vector<Vec2d> circle;
    for (int i = 0; i < num_points; ++i) {
      double angle = 2 * M_PI * i / num_points;
      circle.emplace_back(Vec2d(center.y + radius * cos(angle),
                                center.z + radius * sin(angle)));
    }
    return circle;
  }

  std::vector<Vec2d> drawRect(Vec2d center, double width, double height) {
    std::vector<Vec2d> rect;
    rect.emplace_back(Vec2d(center.y - width / 2, center.z - height / 2));
    rect.emplace_back(Vec2d(center.y - width / 2, center.z + height / 2));
    rect.emplace_back(Vec2d(center.y + width / 2, center.z + height / 2));
    rect.emplace_back(Vec2d(center.y + width / 2, center.z - height / 2));
    return rect;
  }
};

TEST_F(PolygonTest, line) {
  Polygon2d box_one(PolygonType::Box, {10, 10}, 20, 10, 0);
  Polygon2d region_rect(PolygonType::Region, rectangle, {});
  Polygon2d region_valley(PolygonType::Region, valley, {});
  EXPECT_EQ(PolygonType::Box, box_one.type());
  EXPECT_EQ(PolygonType::Region, region_valley.type());
  GEOM_EQ(box_one.area(), 200);
  GEOM_EQ(region_rect.area(), 40000);
  GEOM_EQ(region_valley.area(), 7500);

  auto top_line = box_one.f_line();
  GPOINT_EQ(top_line[0], {5, 20});
  GPOINT_EQ(top_line[1], {15, 20});

  auto back_line = box_one.b_line();
  GPOINT_EQ(back_line[0], {15, 0});
  GPOINT_EQ(back_line[1], {5, 0});

  innovusion::geometry::Gsegment my_line = {{-5.0, 10.0}, {10.0, 10.0}};
  EXPECT_EQ(box_one.front_distance_to(my_line), 10.0);
  my_line = {{5.0, 10.0}, {15.0, 10.0}};
  EXPECT_EQ(box_one.front_distance_to(my_line), 10.0);
  my_line = {{-20.0, 10.0}, {100.0, 10.0}};
  EXPECT_EQ(box_one.front_distance_to(my_line), 10.0);
  my_line = {{0.0, 0.0}, {5.0, 5.0}};
  EXPECT_EQ(box_one.front_distance_to(my_line), 15.0);
  my_line = {{0.0, 0.0}, {5.0, -5.0}};
  EXPECT_EQ(box_one.front_distance_to(my_line), sqrt(425));
  my_line = {{-100.0, 50.0}, {100.0, 50.0}};
  EXPECT_EQ(box_one.front_distance_to(my_line), 30.0);
}

TEST_F(PolygonTest, region_with_holes_and_valid) {
  Polygon2d region_hole_one(PolygonType::Region, rectangle, {valley});
  std::shared_ptr<Polygon2d> rh_ptr =
      std::make_shared<Polygon2d>(region_hole_one);

  GEOM_EQ(region_hole_one.area(), 32500);
  Polygon2d should_invalid(PolygonType::Region, rectangle, {reversed_c});
  EXPECT_FALSE(should_invalid.isValid());

  Polygon2d box_two(PolygonType::Box, {-110, 0}, 20, 10, 0);
  EXPECT_EQ(box_two.iouSelf(rh_ptr), 0);
  EXPECT_EQ(box_two.iouTarget(rh_ptr), 0);

  Polygon2d box_three(PolygonType::Box, {-50, 0}, 20, 10, 0);
  EXPECT_EQ(box_three.iouSelf(rh_ptr), 50);
  EXPECT_EQ(box_three.iouTarget(rh_ptr), 0);

  Polygon2d box_four(PolygonType::Box, {-25, 0}, 20, 10, 0);
  EXPECT_EQ(box_four.iouSelf(rh_ptr), 0);
  EXPECT_EQ(box_four.iouTarget(rh_ptr), 0);

  Polygon2d box_five(PolygonType::Box, {0, 0}, 20, 10, 0);
  EXPECT_EQ(box_five.iouSelf(rh_ptr), 38);
  EXPECT_EQ(box_five.iouTarget(rh_ptr), 0);

  Polygon2d box_six(PolygonType::Box, {0, 0}, 20, 10, 9000);
  EXPECT_EQ(box_six.iouSelf(rh_ptr), 13);
  EXPECT_EQ(box_six.iouTarget(rh_ptr), 0);
  auto top_line = box_six.f_line();
  GPOINT_EQ(top_line[0], {10, 5});
  GPOINT_EQ(top_line[1], {10, -5});

  Polygon2d box_six_e(PolygonType::Box, {-10, -20}, 20, 10, 9000);
  top_line = box_six_e.f_line();
  GPOINT_EQ(top_line[0], {0, -15});
  GPOINT_EQ(top_line[1], {0, -25});

  Polygon2d box_seven(PolygonType::Box, {0, 0}, 20, 10, 4500);
  EXPECT_EQ(box_seven.iouSelf(rh_ptr), 25);
  EXPECT_EQ(box_seven.iouTarget(rh_ptr), 0);

  Polygon2d box_eight(PolygonType::Box, {50, 50}, 100, 100, 0);
  EXPECT_EQ(box_eight.iouSelf(rh_ptr), 88);
  EXPECT_EQ(box_eight.iouTarget(rh_ptr), 27);
}

TEST_F(PolygonTest, point_filter) {
  Polygon2d region_rect(PolygonType::Region, rectangle, {});
  EXPECT_TRUE(region_rect.within({0, 0}));
  EXPECT_FALSE(region_rect.within({-100, 50}));
  EXPECT_TRUE(region_rect.covered({-100, 50}));
}

TEST_F(PolygonTest, flexible_test) {
  std::vector<Vec2d> outside = {
      {-100, -100}, {-100, 100}, {100, 100}, {100, -100}};
  std::vector<Vec2d> hole_one = {{-10, -10}, {-10, 10}, {10, 10}, {10, -10}};
  std::vector<Vec2d> hole_two = {{20, 20}, {0, 40}, {40, 40}};

  Polygon2d region_outside(PolygonType::Region, outside, {});
  Polygon2d region_hole_one(PolygonType::Region, hole_one, {});
  Polygon2d region_hole_two(PolygonType::Region, hole_two, {});
  EXPECT_TRUE(region_outside.isValid());
  EXPECT_TRUE(region_hole_one.isValid());
  EXPECT_TRUE(region_hole_two.isValid());
  Polygon2d region_with_two_hole(PolygonType::Region, outside,
                                 {hole_one, hole_two});
  EXPECT_TRUE(region_with_two_hole.isValid());
}

TEST_F(PolygonTest, attributes_and_values) {
  Region2d region_one(1, rectangle, {valley}, {1, 2}, {1, 2});
  int test_value = 0;
  EXPECT_TRUE(region_one.hasAttribute(1));
  EXPECT_TRUE(region_one.getValue(1, &test_value));
  EXPECT_EQ(test_value, 1);

  EXPECT_TRUE(region_one.hasAttribute(2));
  EXPECT_TRUE(region_one.getValue(2, &test_value));
  EXPECT_EQ(test_value, 2);

  EXPECT_TRUE(region_one.setAttributesAndValues({3, 4}, {3, 4}));
  EXPECT_TRUE(region_one.hasAttribute(3));
  EXPECT_TRUE(region_one.getValue(3, &test_value));
  EXPECT_EQ(test_value, 3);

  EXPECT_TRUE(region_one.hasAttribute(4));
  EXPECT_TRUE(region_one.getValue(4, &test_value));
  EXPECT_EQ(test_value, 4);

  EXPECT_FALSE(region_one.setAttributesAndValues({5, 6, 7}, {6, 7}));

  std::vector<uint32_t> c_attributes;
  std::vector<int32_t> c_values;
  region_one.getAttributesAndValues(&c_attributes, &c_values);
  EXPECT_EQ(c_attributes.size(), 2);
  EXPECT_EQ(c_values.size(), 2);
  EXPECT_EQ(c_attributes[0], c_values[0]);
  EXPECT_EQ(c_attributes[1], c_values[1]);
}

// following test is generated for polygon2d

// Tests that a Polygon2d object can be created with valid parameters and is
// valid.
TEST_F(PolygonTest, valid_polygon) {
  Polygon2d polygon(PolygonType::Polygon, {0, 0}, 10.0, 5.0, 9000);
  EXPECT_TRUE(polygon.isValid());
}

// Test that polygon2d type is correct
TEST_F(PolygonTest, type) {
  Polygon2d box_one(PolygonType::Box, {10, 10}, 20, 10, 0);
  Polygon2d region_rect(PolygonType::Region, rectangle, {});
  Polygon2d region_valley(PolygonType::Polygon, valley, {});
  EXPECT_EQ(PolygonType::Box, box_one.type());
  EXPECT_EQ(PolygonType::Region, region_rect.type());
  EXPECT_EQ(PolygonType::Polygon, region_valley.type());
}

TEST_F(PolygonTest, area_related) {
  // Tests that the area of a Polygon2d object is calculated correctly.
  // Attension area precison may be low cause to the small spin angle
  Polygon2d polygon(PolygonType::Polygon, {0, 0}, 10.0, 5.0, 9000);
  GEOM_EQ(polygon.area(), 50.0);

  std::vector<Vec2d> outerPoints1 = {{0, 0}, {10, 0}, {10, 5}, {0, 5}};
  std::vector<Vec2d> outerPoints2 = {{6, 0}, {12, 0}, {12, 3}, {6, 3}};
  PolygonPtr polygon1 = std::make_shared<Polygon2d>(
      PolygonType::Polygon, outerPoints1, std::vector<std::vector<Vec2d>>());
  PolygonPtr polygon2 = std::make_shared<Polygon2d>(
      PolygonType::Polygon, outerPoints2, std::vector<std::vector<Vec2d>>());
  // Tests that the intersection area between two Polygon2d objects is
  // calculated correctly.
  GEOM_EQ(polygon1->calculateIntersectionArea(polygon2), 12.0);
  // Tests that the iou between two Polygon2d objects is calculated correctly.
  GEOM_EQ(polygon1->iou(polygon2), 0.21428571);
  GEOM_EQ(polygon2->iou(polygon1), 0.21428571);
  // Tests that the iouTarget between two Polygon2d objects
  EXPECT_EQ(polygon1->iouTarget(polygon2), 67);
  EXPECT_EQ(polygon2->iouTarget(polygon1), 24);
  // Tests that the iouSelf between two Polygon2d objects
  // is calculated correctly.
  EXPECT_EQ(polygon1->iouSelf(polygon2), 24);
  EXPECT_EQ(polygon2->iouSelf(polygon1), 67);
}

// Tests debugString
TEST_F(PolygonTest, debugString) {
  Polygon2d polygon(PolygonType::Polygon, {0, 0}, 10.0, 5.0, 9000);
  std::string expect_string =
      "(((5, -2.5), (-5, -2.5), (-5, 2.5), (5, 2.5), (5, -2.5)))";
  std::string debug_string = polygon.debugString();
  EXPECT_EQ(debug_string, expect_string);

  std::stringstream output_stream;
  output_stream << polygon;
  std::string output_string = output_stream.str();
  EXPECT_EQ(output_string, expect_string);
}
