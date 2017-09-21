#ifndef USER_INPUT_HPP_
#define USER_INPUT_HPP_

#include <SDL2/SDL.h>
#include <chrono>

#include "Parameters.hpp"
#include "UserInputOutput.hpp"

// standard library includes
#include <memory>

#ifndef WITHOUT_KINECT_LIBRARIES
// forward declarations for kinect
namespace xn {
class Context;
class UserGenerator;
class DepthGenerator;
}
#endif

class UserInput {
private:
    Parameters &parameters;
#ifndef WITHOUT_KINECT_LIBRARIES
    std::unique_ptr<xn::Context> context;
    std::unique_ptr<xn::UserGenerator> userGenerator;
    std::unique_ptr<xn::DepthGenerator> depthGenerator;
#endif

public:
    UserInput(Parameters &parameters);
    ~UserInput();

    void getInput(UserInputOutput &userInputOutput);
};

#endif
