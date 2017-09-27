
#include "test_lbm.hpp"

#include "gtest/gtest.h"
#include <cmath>

#include "LevelDesign/Level.hpp"
#include "LevelDesign/LevelLoader.hpp"

void printFlagField(Array2D<Level::CellType> &flagfield) {

  std::cout << "FLAGS:" << std::endl << std::endl;
  int width = flagfield.width();
  int height = flagfield.height();

  for (int j=height-1; j>=0; j--) {
    for (int i=0; i<width; i++) {
      std::cout << flagfield.value(i,j) << " ";
    }
    std::cout << std::endl;
  }
}

void printVelocities(LatticeBoltzmannInput &input) {

  std::cout << "Input velocities:" << std::endl << std::endl;
  int width = input.velocities.width();
  int height = input.velocities.height();

  for (int j=height-1; j>=0; j--) {
    for (int i=0; i<width; i++) {
      float u = input.velocities.value(i,j)[0];
      float v = input.velocities.value(i,j)[1];
      std::cout << u << "," << v << " ";
    }
    std::cout << std::endl;
  }
}

void printVelocities(LatticeBoltzmannOutput &output) {

  std::cout << "Output velocities:" << std::endl << std::endl;
  int width = output.velocity.width();
  int height = output.velocity.height();

  for (int j=height-1; j>=0; j--) {
    for (int i=0; i<width; i++) {
      float u = output.velocity.value(i,j)[0];
      float v = output.velocity.value(i,j)[1];
      std::cout << "velocity (" << i << ", " << j << ") is: "<< u << ", " << v << " " << std::endl;
    }
    std::cout << std::endl;
  }
}

void printFIs(Array2D<FICell> &fi) {
  int width = fi.width();
  int height = fi.height();
  std::cout << "FIs:" << std::endl << std::endl;
  for (int j=height-1; j>=0; j--) {
    for (int i=0; i<width; i++) {
      std::cout << i << j << " {";
      for (int k=0; k<9; k++) {
        std::cout << fi.value(i,j)[k] << ",";
      }
      std::cout << "}  " << std::endl;
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

      if ((fabs(me-left) > threshold) || (fabs(me-down) > threshold)) {
          return false;
      }
    }
  }
  return true;
}


TEST(LBMTest, CollideStep1x1) {
  LevelLoader levelLoader("test/level5x5.txt");
  Level level;
  levelLoader.load(level);
  printFlagField(level.matrix);

  LatticeBoltzmannInput input;
  printFlagField(input.flagfield);
  LatticeBoltzmannOutput output;
  //output.density.value(0,0) = 0;
  //output.velocity.value(0,0) = glm::vec2(0,0);

  LatticeBoltzmann sut(level);
  sut.compute(input, output);
  //std::cout << output.density.value(0,0) << std::endl;
  //std::cout << output.velocity.value(0,0)[0] << std::endl;
  //std::cout << output.velocity.value(0,0)[1] << std::endl;
  EXPECT_EQ(5,level.width);
}

TEST(LBMTest, EquilibriumTrue) {
  LevelLoader levelLoader("test/level5x5.txt");
  Level level;
  levelLoader.load(level);

  LatticeBoltzmannInput input;
  LatticeBoltzmannOutput output;
  LatticeBoltzmann sut(level);

  input.velocities = Array2D<glm::vec2>{ level.width, level.height };
  input.flagfield = level.matrix;
  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      input.velocities.value(i,j)[0] = 0.1;
    }
  }
  //printVelocities(input);
  //printFIs(output.afterstream);
  sut.compute(input, output);
  //printFIs(output.prestream);
  //printFIs(output.afterstream);
  printVelocities(output);
  //for (int i =0; i < 1000; ++i){
  //   sut.compute(input, output);
  //}
  //printVelocities(output);
  //printFIs(output.afterstream);
  EXPECT_EQ(5,level.width);
}
