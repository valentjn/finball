#include "gtest/gtest.h"
#include "../src/LatticeBoltzmann.hpp"

TEST(LBMTest, CollideStep1x1) {

  Level level;
  LatticeBoltzmannInput input;
  LatticeBoltzmannOutput output;

  level.width=1;
  level.height=1;
  output.velocity = std::make_unique<Array2D<glm::vec2>>(1,1);
  output.velocity->value(0,0) = glm::vec2(0,0);
  output.density = std::make_unique<Array2D<float>>(1,1);
  output.density->value(0,0) = 0;

  LatticeBoltzmann sut(level);
  sut.compute(input, output);
  std::cout << output.density->value(0,0) << std::endl;
  EXPECT_EQ(1,level.width);
}

TEST(LatticBoltzmannTest, EquilibriumTrue) {

  Level level;
  LatticeBoltzmannInput input;
  LatticeBoltzmannOutput output;

  level.width=1;
  level.height=1;
  output.velocity = std::make_unique<Array2D<glm::vec2>>(1,1);
  output.velocity->value(0,0) = glm::vec2(0,0);
  output.density = std::make_unique<Array2D<float>>(1,1);
  output.density->value(0,0) = 0;

  LatticeBoltzmann sut(level);
  sut.compute(input, output);
  std::cout << output.density->value(0,0) << std::endl;
  EXPECT_EQ(1,level.width);
}
