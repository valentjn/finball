#include "gtest/gtest.h"
#include "../src/LatticeBoltzmann.hpp"

void printFlagField(LatticeBoltzmannInput &input) {

  std::cout << "FLAGS:" << std::endl << std::endl;
  int width = input.flagfield->width();
  int height = input.flagfield->height();

  for (int i=0; i<width; i++) {
    for (int j=0; j<height; j++) {
      std::cout << input.flagfield->value(i,j) << " ";
    }
    std::cout << std::endl;
  }
}

void printVelocities(LatticeBoltzmannInput &input) {

  std::cout << "Input velocities:" << std::endl << std::endl;
  int width = input.velocities->width();
  int height = input.velocities->height();

  for (int i=0; i<width; i++) {
    for (int j=0; j<height; j++) {
      float u = input.velocities->value(i,j)[0];
      float v = input.velocities->value(i,j)[1];
      std::cout << u << "," << v << " ";
    }
    std::cout << std::endl;
  }
}

void printVelocities(LatticeBoltzmannOutput &output) {

  std::cout << "Output velocities:" << std::endl << std::endl;
  int width = output.velocity.width();
  int height = output.velocity.height();

  for (int i=0; i<width; i++) {
    for (int j=0; j<height; j++) {
      float u = output.velocity.value(i,j)[0];
      float v = output.velocity.value(i,j)[1];
      std::cout << u << "," << v << " ";
    }
    std::cout << std::endl;
  }
}

void printFIs(Array2D<FICell> &fi) {
  int width = fi.width();
  int height = fi.height();
  std::cout << "FIs:" << std::endl << std::endl;
  for (int i=0; i<width; i++) {
    for (int j=0; j<height; j++) {
      std::cout << "{";
      for (int k=0; k<9; k++) {
        std::cout << fi.value(i,j)[k] << ",";
      }
      std::cout << "}  ";
    }
    std::cout << std::endl;
  }
}
void renderVelocities(LatticeBoltzmannOutput &output) {

}

float computeTotalFI(Array2D<FICell> &fi) {
  int width = fi.width();
  int height = fi.height();
  float sum = 0;
  for (int i=0; i<width; i++) {
    for (int j=0; j<height; j++) {
      for (int k=0; k<9; k++) {
        sum += fi.value(i,j)[k];
      }
    }
  }
  return sum;
}

float computeTotalMass(Array2D<float> &rho) {
  int width = rho.width();
  int height = rho.height();
  float sum = 0;
  for (int i=0; i<width; i++) {
    for (int j=0; j<height; j++) {
      sum += rho.value(i,j);
    }
  }
  return sum;
}

bool isSmooth(Array2D<float> &rho, float threshold) {
  int width = rho.width();
  int height = rho.height();

  for (int i=1; i<width; i++) {
    for (int j=1; j<height; j++) {

      float me = rho.value(i,j);
      float left = rho.value(i-1,j);
      float down = rho.value(i,j-1);

      if ((abs(me-left) > threshold) || (abs(me-down) > threshold)) {
          return false;
      }
    }
  }
  return true;
}


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

  Level level("test/level5x5.txt");
  LatticeBoltzmannInput input(level);
  LatticeBoltzmannOutput output(level);
  LatticeBoltzmann sut(level);


  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      input.velocities->value(i,j)[0] = 0.1;
    }
  }
  printVelocities(input);
  printFIs(output.afterstream);
  sut.compute(input, output);
  printFIs(output.afterstream);
  printVelocities(output);
  EXPECT_EQ(5,level.width);
}



