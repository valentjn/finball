#include "gtest/gtest.h"

#include "test_lbm.hpp"

#include "RigidBody/RigidBodyPhysics.hpp"

// if the rigid body is in the middle in the beginning,
// then nothing should move in the first few time steps
// TODO do the same with lb time steps executed
TEST(RigidBodyTest, static64) {

  Level level("data/testLevel.txt");
  level.rigidBodies = vector<RigidBody>();
  level.rigidBodies.push_back(RigidBody(666, level.width/2, level.height/2, false));

  RigidBodyPhysicsOutput output(level);
  RigidBodyPhysicsInput input;

  UserInputOutput userInputOutput;
  LatticeBoltzmannOutput latticeBoltzmannOutput(level);
  input = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);

  for (int i = 0; i<level.width ; ++i){

      for (int j = 0; j<level.height ; ++j){
          latticeBoltzmannOutput.density.value(i,j) = 1;
          latticeBoltzmannOutput.velocity.value(i,j) = glm::vec2(0,0);
      }
  }


  RigidBodyPhysics sut(level);
  sut.setGravity(false);
// for some reason, the first field is zero-initialized
  sut.compute(input, output);

  Array2D<Level::CellType> init_flagfield = output.grid_objects;

  for (int i = 0; i < level.width/2 - 5 ; ++i){
      sut.compute(input, output);
  }

  bool changed = true;
  for (int i = 0; i<level.width ; ++i){

      for (int j = 0; j<level.height ; ++j){
        if (init_flagfield.value(i,j) != output.grid_objects.value(i,j)) {
			changed = false;
			break;
		}
      }
  }
  
  EXPECT_TRUE(changed);
  if(! changed){
    printFlagField(output.grid_objects);
  }
}


// if the rigid body is in the middle in the beginning,
//AND we have gravity,
// then something should move in the first few time steps
TEST(RigidBodyTest, gravity64) {

  Level level("data/testLevel.txt");
  level.rigidBodies = vector<RigidBody>();
  level.rigidBodies.push_back(RigidBody(666, level.width/2, level.height/2, false));

  RigidBodyPhysicsOutput output(level);
  RigidBodyPhysicsInput input;

  UserInputOutput userInputOutput;
  LatticeBoltzmannOutput latticeBoltzmannOutput(level);
  input = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);

  for (int i = 0; i<level.width ; ++i){

      for (int j = 0; j<level.height ; ++j){
          latticeBoltzmannOutput.density.value(i,j) = 1;
          latticeBoltzmannOutput.velocity.value(i,j) = glm::vec2(0,0);
      }
  }


  RigidBodyPhysics sut(level);
  sut.setGravity(true);
// for some reason, the first field is zero-initialized
  sut.compute(input, output);

  Array2D<Level::CellType> init_flagfield = output.grid_objects;

  for (int i = 0; i < level.width/2 - 5 ; ++i){
      sut.compute(input, output);
  }
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
}


// if the rigid body is above the horizontal slope in the beginning,
//AND we have gravity,
// then the x coordinate should not change in the first few time steps
TEST(RigidBodyTest, stop64) {

  Level level("data/testLevel.txt");
  level.rigidBodies = vector<RigidBody>();
  level.rigidBodies.push_back(RigidBody(666, level.width/4, level.height*0.9, false));

  RigidBodyPhysicsOutput output(level);
  RigidBodyPhysicsInput input;

  UserInputOutput userInputOutput;
  LatticeBoltzmannOutput latticeBoltzmannOutput(level);
  input = RigidBodyPhysicsInput(userInputOutput, latticeBoltzmannOutput);

  for (int i = 0; i<level.width ; ++i){

      for (int j = 0; j<level.height ; ++j){
          latticeBoltzmannOutput.density.value(i,j) = 1;
          latticeBoltzmannOutput.velocity.value(i,j) = glm::vec2(0,0);
      }
  }


  RigidBodyPhysics sut(level);
  sut.setGravity(true);
// for some reason, the first field is zero-initialized
  sut.compute(input, output);

  Array2D<Level::CellType> init_flagfield = output.grid_objects;

  for (int i = 0; i < level.width ; ++i){
      sut.compute(input, output);
  }

  bool changed = false;
  for (int i = 0; i<level.width ; ++i){

      for (int j = 0; j<level.height ; ++j){
        if(init_flagfield.value(i,j) != output.grid_objects.value(i,j)){
            changed = true;
        }
      }
  }

  EXPECT_TRUE(changed);
  //if(! changed){
  printFlagField(init_flagfield);
  printFlagField(output.grid_objects);
  //}
}
