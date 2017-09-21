#include "gtest/gtest.h"
#include "../src/LatticeBoltzmann.hpp"
#include "../src/LatticeBoltzmannInput.hpp"
#include "../src/LatticeBoltzmannOutput.hpp"

TEST(LBMTest, CollideStep1x1) {
  EXPECT_TRUE(true);
  LatticeBoltzmann sut(nullptr, nullptr);
  LatticeBoltzmannInput input(nullptr, nullptr);
  LatticeBoltzmannOutput output;
  sut.compute(input, output);

  

}
