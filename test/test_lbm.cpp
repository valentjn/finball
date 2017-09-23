#include "gtest/gtest.h"
#include "../src/LatticeBoltzmann.hpp"

TEST(LBMTest, CollideStep1x1) {

  Level level("data/testLevel.txt");
  level.width=1;
  level.height=1;

  LatticeBoltzmannInput input(level);
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

TEST(LBMTest, EquilibriumTrue) {

  
  Level level("data/testLevel.txt");
  LatticeBoltzmannInput input(level);
  LatticeBoltzmannOutput output(level);

  LatticeBoltzmann sut(level);

  sut.compute(input, output);
  for (int i=0; i<64; i++) {
    for (int j=0; j<64; j++) {
        std::cout << output.velocity.value(i,j)[0] << " ";
    }
    std::cout << std::endl;
  }
  EXPECT_EQ(64,level.width);
}
