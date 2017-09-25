#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include <memory>

#include <glm/vec3.hpp>

#include "Array2D.hpp"
#include "LatticeBoltzmann/FICell.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmann/LatticeBoltzmannOutput.hpp"
#include "Level.hpp"

using namespace glm;

class LatticeBoltzmann {
private:
    Level &level;

    // previous f_i field
    Array2D<FICell> fi_Old;

    // processed f_i field
    Array2D<FICell> fi_New;

    // quadrature weights for approximating equilibrium distribution
    float w[9] = {4. / 9.,  1. / 9.,  1. / 9.,  1. / 9., 1. / 9.,
                  1. / 36., 1. / 36., 1. / 36., 1. / 36.};
    int cx[9] = {0, 1, 0, -1, 0, 1, -1, -1, 1};
    int cy[9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
    int opp[9] = {0, 3, 4, 1, 2, 7, 8, 5, 6};

    bool isBoundaryValid(const Array2D<Level::CellType> &flagfield) {
        int width = flagfield.width();
        int height = flagfield.height();
        bool result = true;
        for (int i=0; i<width; i++) {  // top and bottom
          if (flagfield.value(i,0) == Level::CellType::FLUID) result=false;
          if (flagfield.value(i,height-1) == Level::CellType::FLUID) result=false;
        }
        for (int j=1; j<height-1; j++) {  // left and right sides
          if (flagfield.value(0,j) == Level::CellType::FLUID) result=false;
          if (flagfield.value(width-1,j) == Level::CellType::FLUID) result=false;
        }
        if (!result) {
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
        return result;
    }
public:
    LatticeBoltzmann(Level &level)
        : level(level), fi_Old(level.width, level.height), fi_New(level.width, level.height) {
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                // set initial values
                for (int z = 0; z < 9; z++) {
                    fi_New.value(x, y)[z] = w[z] * 0.1 / (0.1 + x);
                    fi_Old.value(x, y)[z] = w[z] * 0.1 / (0.1 + x);
                }
            }
        }
    }

    void compute(const LatticeBoltzmannInput &input, LatticeBoltzmannOutput &output) {

        // Check flag field
        assert(isBoundaryValid(input.flagfield));

        //######################################## Collision
        //####################################################
        for (int y = 1; y < level.height - 1; ++y) {
            for (int x = 1; x < level.width - 1; ++x) {
                // check for boundary
                if (input.flagfield.value(x, y) == Level::CellType::FLUID) {

                    float rho = 0.0;
                    float velx = 0.0;
                    float vely = 0.0;

                    // calculate density
                    rho = fi_Old.value(x, y)[0] + fi_Old.value(x, y)[1] + fi_Old.value(x, y)[2] +
                          fi_Old.value(x, y)[3] + fi_Old.value(x, y)[4] + fi_Old.value(x, y)[5] +
                          fi_Old.value(x, y)[6] + fi_Old.value(x, y)[7] + fi_Old.value(x, y)[8];

                    // TODO later          density->get(x, y, vector::x) = rho ;
                    const float rhoinv = 1.0 / rho;

                    velx =
                        ((fi_Old.value(x, y)[5] + fi_Old.value(x, y)[1] + fi_Old.value(x, y)[8]) -
                         (fi_Old.value(x, y)[6] + fi_Old.value(x, y)[3] + fi_Old.value(x, y)[7])) *
                        rhoinv;

                    // TODO later   velocity->get(x, y, vector::x) = velx ;
                    vely =
                        ((fi_Old.value(x, y)[6] + fi_Old.value(x, y)[2] + fi_Old.value(x, y)[5]) -
                         (fi_Old.value(x, y)[7] + fi_Old.value(x, y)[4] + fi_Old.value(x, y)[8])) *
                        rhoinv;

                    // TODO later              velocity->get(x, y, vector::y) = vely ;

                    const float velxx = velx * velx;
                    const float velyy = vely * vely;

                    // TODO need to set omega
                    float omega = 1.0;
                    // For the center
                    float feqC = w[0] * rho * (1.0 - 3.0 * (velxx + velyy) * 0.5);
                    fi_Old.value(x, y)[0] += omega * (feqC - fi_Old.value(x, y)[0]);

                    // TODO later what is ax velocity of obsatcle
                    float ax = 0;
                    float ay = 0;
                    // For the east distribution function
                    float feqE =
                        w[1] * rho * (1.0 + 3.0 * velx + 4.5 * velxx - 1.5 * (velxx + velyy));
                    fi_Old.value(x, y)[1] +=
                        omega * (feqE - fi_Old.value(x, y)[1]) + 3.0 * w[1] * rho * ax;

                    // for the North East Direction
                    float feqNE =
                        w[5] * rho * (1.0 + 3.0 * (velx + vely) +
                                      4.5 * (velx + vely) * (velx + vely) - 1.5 * (velxx + velyy));
                    fi_Old.value(x, y)[5] +=
                        omega * (feqNE - fi_Old.value(x, y)[5]) + 3.0 * w[1] * rho * (ax + ay);

                    // For the north direction
                    float feqN =
                        w[2] * rho * (1.0 + 3.0 * vely + 4.5 * velyy - 1.5 * (velxx + velyy));
                    fi_Old.value(x, y)[2] +=
                        omega * (feqN - fi_Old.value(x, y)[2]) + 3.0 * w[1] * rho * ay;

                    // For the North west direction
                    float feqNW = w[6] * rho *
                                  (1.0 + 3.0 * (-velx + vely) +
                                   4.5 * (-velx + vely) * (-velx + vely) - 1.5 * (velxx + velyy));
                    fi_Old.value(x, y)[6] +=
                        omega * (feqNW - fi_Old.value(x, y)[6]) + 3.0 * w[1] * rho * (-ax + ay);

                    // for the west direction
                    float feqW =
                        w[3] * rho * (1.0 - 3.0 * velx + 4.5 * velxx - 1.5 * (velxx + velyy));
                    fi_Old.value(x, y)[3] +=
                        omega * (feqW - fi_Old.value(x, y)[3]) + 3.0 * w[1] * rho * (-ax);

                    // for the south west
                    float feqSW =
                        w[7] * rho * (1.0 - 3.0 * (velx + vely) +
                                      4.5 * (velx + vely) * (velx + vely) - 1.5 * (velxx + velyy));
                    fi_Old.value(x, y)[7] +=
                        omega * (feqSW - fi_Old.value(x, y)[7]) + 3.0 * w[1] * rho * (-ax - ay);

                    // for the South direction
                    float feqS =
                        w[4] * rho * (1.0 - 3.0 * vely + 4.5 * velyy - 1.5 * (velxx + velyy));
                    fi_Old.value(x, y)[4] +=
                        omega * (feqS - fi_Old.value(x, y)[4]) + 3.0 * w[1] * rho * (-ay);

                    // for the south east direction
                    float feqSE =
                        w[8] * rho * (1.0 + 3.0 * (velx - vely) +
                                      4.5 * (velx - vely) * (velx - vely) - 1.5 * (velxx + velyy));
                    fi_Old.value(x, y)[8] +=
                        omega * (feqSE - fi_Old.value(x, y)[8]) + 3.0 * w[1] * rho * (ax - ay);
                }
            }
        }
        //#################################### End of Collision
        //##################################################

        // output fi prestreaming
        for (int y = 0; y < level.height; ++y) {
            for (int x = 0; x < level.width; ++x) {
                for (int z = 0; z < 9; ++z) {
                    output.prestream.value(x, y)[z] = fi_Old.value(x, y)[z];
                }
            }
        }

        // set f_i in obstacles to 0
        for (int y = 0; y < level.height; ++y) {
            for (int x = 0; x < level.width; ++x) {
                if (input.flagfield.value(x, y) == Level::CellType::OBSTACLE) {
                    for (int i = 0; i < 9; ++i) {
                        fi_Old.value(x, y)[i] = 0.0;
                    }
                }
            }
        }

        //#################################### Streaming
        //############################################
        for (int y = 1; y < level.height - 1; ++y) {
            for (int x = 1; x < level.width - 1; ++x) {
                // check for boundary
                if (input.flagfield.value(x, y) == Level::CellType::FLUID) {
                    fi_New.value(x, y)[0] = fi_Old.value(x, y)[0];
                    fi_New.value(x + 1, y)[1] = fi_Old.value(x, y)[1];
                    fi_New.value(x + 1, y + 1)[5] = fi_Old.value(x, y)[5];
                    fi_New.value(x, y + 1)[2] = fi_Old.value(x, y)[2];
                    fi_New.value(x - 1, y + 1)[6] = fi_Old.value(x, y)[6];
                    fi_New.value(x - 1, y)[3] = fi_Old.value(x, y)[3];
                    fi_New.value(x - 1, y - 1)[7] = fi_Old.value(x, y)[7];
                    fi_New.value(x, y - 1)[4] = fi_Old.value(x, y)[4];
                    fi_New.value(x + 1, y - 1)[8] = fi_Old.value(x, y)[8];
                }
            }
        }

        // now fi_New contains the streamed and collided values
        //#################################### End of Streaming
        //##########################################

        // boundary handling

        for (int y = 0; y < level.height; ++y) {
            for (int x = 0; x < level.width; ++x) {
                // Stream back the velocities streamed into obstacles.
                if (input.flagfield.value(x, y) == Level::CellType::OBSTACLE) {
                    for (int z = 1; z < 9; ++z) {
                        if (fi_New.value(x, y)[z] != 0.0) {
                            fi_New.value(x + cx[opp[z]], y + cy[opp[z]])[opp[z]] =
                                fi_New.value(x, y)[z];
                            fi_New.value(x, y)[z] = 0.0;
                        }
                    }
                }
                // inflow
                else if (input.flagfield.value(x, y) == Level::CellType::INFLOW) {
                    for (int z = 0; z < 9; z++) {
                        // TODO adjust inflow values
                        fi_New.value(x, y)[z] = w[z] * 0.1;
                        fi_Old.value(x, y)[z] = w[z] * 0.1;
                        if (0 <= (x + cx[z]) && (x + cx[z]) < level.width && 0 <= (y + cy[z]) &&
                            (y + cy[z]) < level.height &&
                            input.flagfield.value(x + cx[z], y + cy[z]) ==
                                Level::CellType::FLUID) {
                            fi_New.value(x + cx[z], y + cy[z])[z] = fi_New.value(x, y)[z];
                        }
                    }
                }
                // outflow
                else if (input.flagfield.value(x, y) == Level::CellType::OUTFLOW) {
                    for (int z = 0; z < 9; z++) {
                        // TODO adjust outflow values
                        fi_New.value(x, y)[z] = 0;
                        fi_Old.value(x, y)[z] = 0;
                        if (0 <= (x + cx[z]) && (x + cx[z]) < level.width && 0 <= (y + cy[z]) &&
                            (y + cy[z]) < level.height &&
                            input.flagfield.value(x + cx[z], y + cy[z]) ==
                                Level::CellType::FLUID) {
                            fi_New.value(x + cx[z], y + cy[z])[z] = fi_New.value(x, y)[z];
                        }
                    }
                }
            }
        }
        //#################################### Output #######################################
        // Calculate macroscopic quantities for the output
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                output.density.value(x, y) = 0;
                for (int i = 0; i < 9; i++) {
                    output.density.value(x, y) += fi_New.value(x, y)[i]; // density
                }
                output.velocity.value(x, y)[0] = fi_New.value(x, y)[1] - fi_New.value(x, y)[3] +
                                                 fi_New.value(x, y)[5] - fi_New.value(x, y)[6] -
                                                 fi_New.value(x, y)[7] +
                                                 fi_New.value(x, y)[8]; // x Velocity
                output.velocity.value(x, y)[1] = fi_New.value(x, y)[2] - fi_New.value(x, y)[4] +
                                                 fi_New.value(x, y)[5] + fi_New.value(x, y)[6] -
                                                 fi_New.value(x, y)[7] -
                                                 fi_New.value(x, y)[8]; // y Velocity
            }
        }

        // output fi afterstreaming
        for (int y = 0; y < level.height; ++y) {
            for (int x = 0; x < level.width; ++x) {
                for (int z = 0; z < 9; ++z) {
                    output.afterstream.value(x, y)[z] = fi_New.value(x, y)[z];
                }
            }
        }

        // Set fi_old = fi_new
        // TODO: Swap pointers instead of values
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                for (int z = 0; z < 9; ++z) {
                    output.prestream.value(x, y)[z] = fi_Old.value(x, y)[z];
                    output.afterstream.value(x, y)[z] = fi_New.value(x, y)[z];
                    fi_Old.value(x, y)[z] = fi_New.value(x, y)[z];
                }
            }
        } 

        // Pass some dummy values downstream until the real values work
        // TODO remove
        //for (int y = 0; y < level.height; y++) {
          //for (int x = 0; x < level.width; x++) {
            //output.velocity.value(x,y) = { (x + 0.0)/level.width , (y + 0.0)/level.height };
          //}
        //}
    }
};

#endif
