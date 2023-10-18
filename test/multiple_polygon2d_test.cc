/*
 * Copyright [2023] <Innovusion Inc.>
 * @LastEditTime: 2023-09-13
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/test/multiple_polygon2d_test.cc
 */

/*
TODO:
  1. Function add may need more condition coverage
  2. Function isUseful need one more condition coverage
*/
#include "multiple_polygon2d.h"

#include <gtest/gtest.h>

using innovusion::geometry::MultiplePolygon2d;
using innovusion::geometry::Polygon2d;
using innovusion::geometry::PolygonType;
using innovusion::geometry::Roi2d;
using innovusion::geometry::Vec2d;

class MPolygonTest : public ::testing::Test {
 protected:
  void SetUp() override {}

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

// should return true when adding valid polygon
TEST_F(MPolygonTest, add_valid_polygon) {
  MultiplePolygon2d multiplePolygon;
  std::vector<Vec2d> outer = drawCircle({0.0, 0.0}, 20.0, 100);
  std::vector<std::vector<Vec2d>> inners = {drawCircle({5.0, 5.0}, 5.0, 50)};
  EXPECT_TRUE(multiplePolygon.add(0, outer, inners));
}

// should return false when adding invalid polygon
TEST_F(MPolygonTest, add_invalid_polygon) {
  MultiplePolygon2d multiplePolygon;
  std::vector<Vec2d> outer = drawCircle({0.0, 0.0}, 20.0, 100);
  std::vector<std::vector<Vec2d>> inners = {drawCircle({10.0, 0.0}, 20.0, 100)};
  EXPECT_FALSE(multiplePolygon.add(0, outer, inners));
}

// should return true when removing existing polygon
TEST_F(MPolygonTest, remove_existing_polygon) {
  MultiplePolygon2d multiplePolygon;
  std::vector<Vec2d> outer = drawCircle({0.0, 0.0}, 20.0, 100);
  std::vector<std::vector<Vec2d>> inners = {drawCircle({0.0, 0.0}, 10.0, 100)};
  EXPECT_TRUE(multiplePolygon.add(0, outer, inners));
  EXPECT_TRUE(multiplePolygon.remove(0));
}

// should return false when removing non-existing polygon
TEST_F(MPolygonTest, remove_non_existing_polygon) {
  MultiplePolygon2d multiplePolygon;
  std::vector<Vec2d> outer = drawCircle({0.0, 0.0}, 20.0, 100);
  std::vector<std::vector<Vec2d>> inners = {drawCircle({10.0, 0.0}, 5.0, 100)};
  EXPECT_TRUE(multiplePolygon.add(0, outer, inners));
  EXPECT_FALSE(multiplePolygon.remove(1));
}

// should function correctly when adding and removing polygons
TEST_F(MPolygonTest, empty) {
  MultiplePolygon2d multiplePolygon;
  EXPECT_TRUE(multiplePolygon.empty());
  std::vector<Vec2d> outer = drawCircle({0.0, 0.0}, 20.0, 100);
  std::vector<std::vector<Vec2d>> inners = {drawCircle({0.0, 0.0}, 10.0, 100)};
  EXPECT_TRUE(multiplePolygon.add(0, outer, inners));
  EXPECT_FALSE(multiplePolygon.empty());
  EXPECT_TRUE(multiplePolygon.remove(0));
  EXPECT_TRUE(multiplePolygon.empty());
}

TEST_F(MPolygonTest, within_false) {
  MultiplePolygon2d multiplePolygon;
  // should return false when checking within empty MultiplePolygon2d
  Vec2d point = {78, 83};
  EXPECT_FALSE(multiplePolygon.within(point));

  EXPECT_TRUE(multiplePolygon.add(0, drawCircle({0.0, 0.0}, 20.0, 100), {}));
  EXPECT_FALSE(multiplePolygon.within(point));
  EXPECT_TRUE(multiplePolygon.within({5.0, 15.0}));
}

// should return false when checking covered by empty MultiplePolygon2d
TEST_F(MPolygonTest, covered_false) {
  MultiplePolygon2d multiplePolygon;
  Vec2d point = {85, 91};
  EXPECT_FALSE(multiplePolygon.covered(point));

  EXPECT_TRUE(multiplePolygon.add(0, drawRect({13.0, 71.0}, 23.0, 37.0), {}));
  EXPECT_FALSE(multiplePolygon.covered(point));
  EXPECT_TRUE(multiplePolygon.covered({15.0, 73.0}));
  // Excatly on the left-top corner
  EXPECT_TRUE(multiplePolygon.covered({1.5, 89.5}));
  // EXcatly on the right edge
  EXPECT_TRUE(multiplePolygon.covered({24.5, 79.0}));
}

// should return true when checking if a polygon overlaps with any polygon in
// MultiplePolygon2d
TEST_F(MPolygonTest, overlaped_true) {
  MultiplePolygon2d multiplePolygon;
  std::vector<Vec2d> outer = drawCircle({0.0, 0.0}, 20.0, 100);
  std::vector<std::vector<Vec2d>> inners = {};
  EXPECT_TRUE(multiplePolygon.add(0, outer, inners));
  std::vector<Vec2d> testOuter = drawCircle({-10.0, 0.0}, 20.0, 100);
  std::vector<std::vector<Vec2d>> testInners = {
      drawCircle({-10.0, 0.0}, 5.0, 50)};
  EXPECT_TRUE(multiplePolygon.overlaped(std::make_shared<Polygon2d>(
      PolygonType::Polygon, testOuter, testInners)));
}

// should calculate the intersection over union (IOU) of a polygon with all
// polygons in MultiplePolygon2d
TEST_F(MPolygonTest, iouTarget) {
  MultiplePolygon2d multiplePolygon;
  std::vector<Vec2d> outer = drawRect({0.0, 0.0}, 40.0, 30.0);
  std::vector<std::vector<Vec2d>> inners = {drawRect({0.0, 0.0}, 10.0, 10.0)};
  EXPECT_TRUE(multiplePolygon.add(0, outer, inners));
  std::vector<Vec2d> testOuter = drawRect({0.0, 0.0}, 20.0, 15.0);
  std::vector<std::vector<Vec2d>> testInners = {};
  int result = multiplePolygon.iouTarget(
      std::make_shared<Polygon2d>(PolygonType::Polygon, testOuter, testInners));
  EXPECT_EQ(result, 67);
}

// should check if a polygon is useful for Roi2d
TEST_F(MPolygonTest, isUseful_polygon) {
  Roi2d roi = Roi2d(true, 50);
  std::vector<Vec2d> outer = drawRect({0.0, 0.0}, 40.0, 30.0);
  std::vector<std::vector<Vec2d>> inners = {drawRect({0.0, 0.0}, 10.0, 10.0)};
  EXPECT_TRUE(roi.add(0, outer, inners));
  std::vector<Vec2d> testOuter = drawRect({0.0, 0.0}, 20.0, 15.0);
  std::vector<std::vector<Vec2d>> testInners = {};
  bool result = roi.isUseful(
      std::make_shared<Polygon2d>(PolygonType::Polygon, testOuter, testInners));
  EXPECT_TRUE(result);
}

// should check if a polygon is useless for Roi2d
TEST_F(MPolygonTest, isUseless_polygon) {
  Roi2d roi = Roi2d(false, 50);
  std::vector<Vec2d> outer = drawRect({0.0, 0.0}, 40.0, 30.0);
  std::vector<std::vector<Vec2d>> inners = {drawRect({0.0, 0.0}, 10.0, 10.0)};
  EXPECT_TRUE(roi.add(0, outer, inners));
  std::vector<Vec2d> testOuter = drawRect({0.0, 0.0}, 5.0, 5.0);
  std::vector<std::vector<Vec2d>> testInners = {};
  bool result = roi.isUseful(
      std::make_shared<Polygon2d>(PolygonType::Polygon, testOuter, testInners));
  EXPECT_TRUE(result);
}

// should check if a point is useful for Roi2d
TEST_F(MPolygonTest, isUseful_point) {
  Roi2d roi = Roi2d(false, 50);
  std::vector<Vec2d> outer = drawRect({0.0, 0.0}, 40.0, 30.0);
  std::vector<std::vector<Vec2d>> inners = {drawRect({0.0, 0.0}, 10.0, 10.0)};
  EXPECT_TRUE(roi.add(0, outer, inners));
  Vec2d point = {0.0, 0.0};
  bool result = roi.isUseful(point);
  EXPECT_TRUE(result);
}

// should check if a point is useless for Roi2d
TEST_F(MPolygonTest, isUseless_point) {
  Roi2d roi = Roi2d(true, 50);
  std::vector<Vec2d> outer = drawRect({0.0, 0.0}, 40.0, 30.0);
  std::vector<std::vector<Vec2d>> inners = {drawRect({0.0, 0.0}, 10.0, 10.0)};
  EXPECT_TRUE(roi.add(0, outer, inners));
  Vec2d point = {-15.0, -7.0};
  bool result = roi.isUseful(point);
  EXPECT_TRUE(result);
}