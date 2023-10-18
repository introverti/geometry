/*
 * Copyright [2022] <Innovusion Inc.>
 * @LastEditTime: 2022-09-12
 * @LastEditors: Tianyun Xuan
 * @FilePath: geometry/test/vec2d_test.cc
 */
#include "vec2d.h"

#include <gtest/gtest.h>

using innovusion::geometry::Vec2d;

#define GEOM_EQ(val_1, val_2)        \
  do {                               \
    EXPECT_NEAR(val_1, val_2, 1e-6); \
  } while (0)

void GPOINT_EQ(Vec2d p1, Vec2d p2) {
  GEOM_EQ(p1.y, p2.y);
  GEOM_EQ(p1.z, p2.z);
}

class PointTest : public ::testing::Test {
 protected:
  void SetUp() override {
    vec2d_one = {2.3, 5.7};
    vec2d_two = boost::geometry::make<Vec2d>(1.5, 2.9);
    boost::geometry::assign_values(vec2d_three, 1, 2);
  }
  Vec2d vec2d_one;
  Vec2d vec2d_two;
  Vec2d vec2d_three;
};

TEST_F(PointTest, Initialization) {
  GEOM_EQ(vec2d_one.y, 2.3);
  GEOM_EQ(vec2d_one.z, 5.7);
  GEOM_EQ(vec2d_two.y, 1.5);
  GEOM_EQ(vec2d_two.z, 2.9);
  GEOM_EQ(vec2d_three.y, 1);
  GEOM_EQ(vec2d_three.z, 2);
}

TEST_F(PointTest, Negative) {
  Vec2d minus_vec2d_one = -vec2d_one;
  Vec2d minus_vec2d_two = -vec2d_two;
  Vec2d minus_vec2d_three = -vec2d_three;
  GEOM_EQ(minus_vec2d_one.y, -2.3);
  GEOM_EQ(minus_vec2d_one.z, -5.7);
  GEOM_EQ(minus_vec2d_two.y, -1.5);
  GEOM_EQ(minus_vec2d_two.z, -2.9);
  GEOM_EQ(minus_vec2d_three.y, -1);
  GEOM_EQ(minus_vec2d_three.z, -2);
}

TEST_F(PointTest, Addition) {
  GEOM_EQ((vec2d_one + vec2d_two).y, 3.8);
  GEOM_EQ((vec2d_one + vec2d_two).z, 8.6);
  GEOM_EQ((vec2d_one + 2.1).y, 4.4);
  GEOM_EQ((vec2d_one + 2.1).z, 7.8);
}

TEST_F(PointTest, Subtraction) {
  GEOM_EQ((vec2d_two - vec2d_one).y, -0.8);
  GEOM_EQ((vec2d_two - vec2d_one).z, -2.8);
  GEOM_EQ((vec2d_two - 33.33).y, -31.83);
  GEOM_EQ((vec2d_two - 33.33).z, -30.43);

  GPOINT_EQ((vec2d_one - vec2d_one), Vec2d(0.0, 0.0));
  GPOINT_EQ((vec2d_two - Vec2d(0.0, 0.0)), vec2d_two);
}

TEST_F(PointTest, Self_add_and_sub) {
  // vec2d_one (2.3,5.7)
  GEOM_EQ((vec2d_one++).y, 2.3);
  // (3.3, 6.7)
  GEOM_EQ((vec2d_one++).z, 6.7);
  // (4.3, 7.7)
  GEOM_EQ((++vec2d_one).y, 5.3);
  // (5.3, 8.7)
  GEOM_EQ((++vec2d_one).z, 9.7);
  // (6.3, 9.7)

  GEOM_EQ((--vec2d_one).z, 8.7);
  GEOM_EQ((--vec2d_one).y, 4.3);
  GEOM_EQ((vec2d_one--).z, 7.7);
  GEOM_EQ((vec2d_one--).y, 3.3);
  GEOM_EQ((vec2d_one).y, 2.3);
  GEOM_EQ((vec2d_one).z, 5.7);

  Vec2d vec(0.0, 0.0);
  GEOM_EQ((vec++).y, 0.0);
  GEOM_EQ((++vec).y, 2.0);
  GEOM_EQ((vec--).y, 2.0);
  GEOM_EQ((--vec).y, 0.0);
  GPOINT_EQ(vec, Vec2d(0.0, 0.0));

  // postfix return a copy
  GEOM_EQ((vec++ ++).z, 0.0);
  GPOINT_EQ(vec, Vec2d(1.0, 1.0));
  GEOM_EQ((vec-- --).y, 1.0);
  GPOINT_EQ(vec, Vec2d(0.0, 0.0));

  // prefix return a reference
  GEOM_EQ((++ ++vec).z, 2.0);
  GEOM_EQ((-- --vec).y, 0.0);

  // prefix than postfix
  GEOM_EQ((++vec++).z, 1.0);
  GEOM_EQ((--vec--).y, 0.0);

  // negative
  GEOM_EQ((-- --vec).y, -2.0);
  GEOM_EQ((-- --vec).z, -4.0);
}

TEST_F(PointTest, Product_scalar_and_vec2dtor) {
  double dot_product = vec2d_one * vec2d_two;
  double cross_product = vec2d_one ^ vec2d_two;
  GEOM_EQ(dot_product, 19.98);
  GEOM_EQ(cross_product, -1.88);
  Vec2d double_vec2d_one = 2 * vec2d_one;
  Vec2d demi_vec2d_two = vec2d_two / 2;
  GEOM_EQ(double_vec2d_one.y, 4.6);
  GEOM_EQ(double_vec2d_one.z, 11.4);
  GEOM_EQ(demi_vec2d_two.y, 0.75);
  GEOM_EQ(demi_vec2d_two.z, 1.45);

  EXPECT_THROW(vec2d_one / 0.0, std::runtime_error);
}

TEST_F(PointTest, Equal) {
  EXPECT_TRUE(vec2d_one == Vec2d(2.3, 5.7));
  EXPECT_FALSE(vec2d_one == vec2d_two);
  EXPECT_FALSE(vec2d_one == vec2d_one - Vec2d(1e-7, 1.0));
  EXPECT_FALSE(vec2d_one == vec2d_one - Vec2d(1.0, 1e-7));
  EXPECT_TRUE(vec2d_one != vec2d_three);
  EXPECT_FALSE(vec2d_three != Vec2d(1.0, 2.0));
}

TEST_F(PointTest, No_const_self_equal) {
  Vec2d temp = vec2d_one;
  Vec2d one_plus_two = vec2d_one + vec2d_two;
  temp += vec2d_two;
  EXPECT_TRUE(temp == one_plus_two);
  temp -= vec2d_two;
  EXPECT_TRUE(temp == vec2d_one);
  temp *= 6;
  EXPECT_TRUE(temp == 6 * vec2d_one);
  temp /= 3;
  EXPECT_TRUE(temp == 2 * vec2d_one);

  // Divide by zero
  EXPECT_THROW(vec2d_one /= 0.0, std::runtime_error);
}

TEST_F(PointTest, Normal) {
  // Null vector
  Vec2d should_be_null(1e-7, 1e-7);
  EXPECT_TRUE(should_be_null.isZeroVector());
  GEOM_EQ(should_be_null.norm(), 0.0);
  GEOM_EQ(should_be_null.normSquare(), 0.0);
  EXPECT_THROW(should_be_null.normalize(), std::runtime_error);

  // Happy pathd
  EXPECT_FALSE(vec2d_one.isZeroVector());
  GEOM_EQ(vec2d_one.normSquare(), 37.78);
  GEOM_EQ(vec2d_one.norm(), 6.14654374);
  vec2d_one.normalize();
  GEOM_EQ(vec2d_one.normSquare(), 1.0);
  GEOM_EQ(vec2d_one.norm(), 1.0);
  GEOM_EQ(vec2d_one.y, 0.37419403);
  GEOM_EQ(vec2d_one.z, 0.92735043);
}

TEST_F(PointTest, Distance_to) {
  GEOM_EQ(vec2d_one.distanceSquareTo(vec2d_two), 8.48);
  GEOM_EQ(vec2d_one.distanceTo(vec2d_two), 2.912044);
  GEOM_EQ(vec2d_one.distanceTo(vec2d_one + 1e-7), 0.0);
}
