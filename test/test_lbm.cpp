#include "gtest/gtest.h"
#include "../src/LatticeBoltzmann/LatticeBoltzmann.hpp"

TEST(LBMTest, CollideStep1x1) {

  Level level("data/testLevel.txt");
  level.width=1;
  level.height=1;

  LatticeBoltzmannInput input;
  LatticeBoltzmannOutput output(level);
  output.density.value(0,0) = 0;
  output.velocity.value(0,0) = glm::vec2(0,0);

  LatticeBoltzmann sut(level);
  sut.compute(input, output);
  std::cout << output.density.value(0,0) << std::endl;
  std::cout << output.velocity.value(0,0)[0] << std::endl;
  std::cout << output.velocity.value(0,0)[1] << std::endl;
  EXPECT_EQ(1,level.width);
}

TEST(LatticBoltzmannTest, EquilibriumTrue) {

  Level level("data/testLevel.txt");
  level.width=1;
  level.height=1;

  LatticeBoltzmannInput input;
  LatticeBoltzmannOutput output(level);
  output.density.value(0,0) = 0;
  output.velocity.value(0,0) = glm::vec2(0,0);

  LatticeBoltzmann sut(level);
  sut.compute(input, output);
  std::cout << "xVelocity=" << output.velocity.value(0,0)[0] << std::endl;
  std::cout << "yVelocity=" << output.velocity.value(0,0)[1] << std::endl;
  std::cout << "Density=" << output.density.value(0,0) << std::endl;
  EXPECT_EQ(1,level.width);
}
