#include <memory>

#include "gtest/gtest.h"

#include "test_lbm.hpp"

#include "LevelDesign/Level.hpp"
#include "LevelDesign/LevelLoader.hpp"
#include "RigidBody/RigidBodyPhysics.hpp"

//set density to 1 and velocity to 0
void initializeLBOutputBoring(LatticeBoltzmannOutput& lbout, int lwidth, int lheight){
	lbout.velocity = Array2D<glm::vec2>{ lwidth, lheight };
	lbout.density = Array2D<float>{ lwidth, lheight };
    for (int i = 0; i<lwidth ; ++i){

        for (int j = 0; j<lheight ; ++j){
            lbout.density.value(i,j) = 1;
            lbout.velocity.value(i,j) = glm::vec2(0,0);
        }
    }
}

void printRBPosition(const Transform& rb){
    std::cout << "(" << rb.position.x << ", " << rb.position.y << ")" << std::endl;
}

void printRBPosition(RigidBodyPhysics& sut, int idx){
    printRBPosition(sut.getRigidBody(idx));
}

bool checkOutsideNotFluid(Array2D<Level::CellType>& flagfield){
    int width = flagfield.width();
    int height = flagfield.height();

    int j=height-1;
    for (int i=0; i<width; i++) {
        if( flagfield.value(i,j) == Level::CellType::FLUID)
            return false;
    }
    j=0;
    for (int i=0; i<width; i++) {
        if( flagfield.value(i,j) == Level::CellType::FLUID)
            return false;
    }
    int i=width-1;
    for (int j=0; j<height; j++) {
        if( flagfield.value(i,j) == Level::CellType::FLUID)
            return false;
    }
    i=0;
    for (int j=0; j<height; j++) {
        if( flagfield.value(i,j) == Level::CellType::FLUID)
            return false;
    }
    return true;
}

void setBallPosition(Level &level, int x, int y) {
    for (const auto &rb : level.rigidBodies) {
        if (rb->id == Level::BALL_ID) {
            rb->position = {x, y};
        }
    }
}


// if the rigid body is in the middle in the beginning,
// then nothing should move in the first few time steps
// TODO do the same with lb time steps executed
TEST(RigidBodyTest, static64) {
  int idx = Level::BALL_ID;

  LevelLoader levelLoader("data/testLevel.txt");
  Level level;

  levelLoader.load(level);
  setBallPosition(level, level.width/2, level.height/2);

  RigidBodyPhysicsOutput output;
  RigidBodyPhysicsInput input;

  UserInputOutput userInputOutput;
  LatticeBoltzmannOutput latticeBoltzmannOutput;
  initializeLBOutputBoring(latticeBoltzmannOutput, level.width, level.height);
  input.process(userInputOutput, latticeBoltzmannOutput);

  RigidBodyPhysics sut(level);
  sut.setGravity(false);

  sut.compute(input, output);

  Array2D<Level::CellType> init_flagfield = output.grid_objects;
  EXPECT_TRUE(checkOutsideNotFluid(init_flagfield));

  Transform before = sut.getRigidBody(idx);

  for (int i = 0; i < level.width/2 - 5 ; ++i){
      sut.compute(input, output);
  }

  Transform after = sut.getRigidBody(idx);

  bool changed = false;
  for (int i = 0; i<level.width ; ++i){

      for (int j = 0; j<level.height ; ++j){
        if (init_flagfield.value(i,j) != output.grid_objects.value(i,j)) {
                        changed = true;
			break;
		}
      }
  }

  EXPECT_FALSE(changed);
  if(changed){
      printFlagField(init_flagfield);
      printFlagField(output.grid_objects);
  }

  EXPECT_FLOAT_EQ(before.position.x, after.position.x);
  EXPECT_FLOAT_EQ(before.position.y, after.position.y);
}


// if the rigid body is in the middle in the beginning,
//AND we have gravity,
// then something should move in the first few time steps
TEST(RigidBodyTest, gravity64) {
  int idx = Level::BALL_ID;

  LevelLoader levelLoader("data/testLevel.txt");
  Level level;

  levelLoader.load(level);
  setBallPosition(level, level.width / 2, level.height / 2);

  RigidBodyPhysicsOutput output;
  RigidBodyPhysicsInput input;

  UserInputOutput userInputOutput;
  LatticeBoltzmannOutput latticeBoltzmannOutput;
  initializeLBOutputBoring(latticeBoltzmannOutput, level.width, level.height);
  input.process(userInputOutput, latticeBoltzmannOutput);

  RigidBodyPhysics sut(level);
  sut.setGravity(true);

  sut.compute(input, output);

  Array2D<Level::CellType> init_flagfield = output.grid_objects;
  EXPECT_TRUE(checkOutsideNotFluid(init_flagfield));

  Transform before = sut.getRigidBody(idx);

  for (int i = 0; i < level.width ; ++i){
      sut.compute(input, output);
  }

  Transform after = sut.getRigidBody(idx);

  bool changed = false;
  for (int i = 0; i<level.width ; ++i){

      for (int j = 0; j<level.height ; ++j){
        if(init_flagfield.value(i,j) != output.grid_objects.value(i,j)){
            changed = true;
            break;
        }
      }
  }

  EXPECT_TRUE(changed);
  if(! changed){
    printFlagField(init_flagfield);
  }

  //EXPECT_NE(before.position.x, after.position.x);
  EXPECT_NE(before.position.y, after.position.y);
}


// if the rigid body is above the horizontal slope in the beginning,
//AND we have gravity,
// then the x coordinate should not change in the first few time steps
TEST(RigidBodyTest, stop64) {
  int idx = Level::BALL_ID;

  LevelLoader levelLoader("data/testLevel.txt");
  Level level;

  levelLoader.load(level);
  setBallPosition(level, level.width/4, level.height*0.9);

  RigidBodyPhysicsOutput output;
  RigidBodyPhysicsInput input;

  UserInputOutput userInputOutput;
  LatticeBoltzmannOutput latticeBoltzmannOutput;
  initializeLBOutputBoring(latticeBoltzmannOutput, level.width, level.height);
  input.process(userInputOutput, latticeBoltzmannOutput);

  RigidBodyPhysics sut(level);
  sut.setGravity(true);

  sut.compute(input, output);

  Array2D<Level::CellType> init_flagfield = output.grid_objects;
  EXPECT_TRUE(checkOutsideNotFluid(init_flagfield));

  Transform before = sut.getRigidBody(idx);
  printRBPosition(before);

  for (int i = 0; i < level.width ; ++i){
      sut.compute(input, output);
  }

  Transform after = sut.getRigidBody(idx);
  printRBPosition(after);

  bool changed = false;
  for (int i = 0; i<level.width ; ++i){

      for (int j = 0; j<level.height ; ++j){
        if(init_flagfield.value(i,j) != output.grid_objects.value(i,j)){
            changed = true;
            break;
        }
      }
  }

  EXPECT_TRUE(changed);
  EXPECT_FLOAT_EQ(before.position.x, after.position.x);
  EXPECT_LE(48, after.position.y);
  if(! changed)
  {
    printFlagField(init_flagfield);
    printFlagField(output.grid_objects);
  }
}
