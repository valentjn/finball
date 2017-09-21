#include "gtest/gtest.h"
#include "../src/LatticeBoltzmann.hpp"

TEST(LBMTest, CollideStep1x1) {

  Parameters parameters;
  Level level;
  LatticeBoltzmannInput input;
  LatticeBoltzmannOutput output;

  level.width=1;
  level.height=1;
  output.matrix = std::make_unique<Array2D<glm::vec3>>(1,1);
  output.matrix->value(0,0) = glm::vec3(0,0,0);

  LatticeBoltzmann sut(parameters, level, false);
  sut.compute(input, output);
  std::cout << output.matrix->value(0,0)[0] << std::endl;
  std::cout << output.matrix->value(0,0)[1] << std::endl;
  std::cout << output.matrix->value(0,0)[2] << std::endl;
  EXPECT_EQ(1,level.width);
}
