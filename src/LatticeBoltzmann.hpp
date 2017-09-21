#ifndef LATTICE_BOLTZMANN_HPP_
#define LATTICE_BOLTZMANN_HPP_

#include <memory>

#include "FICell.hpp"
#include "LatticeBoltzmannInput.hpp"
#include "LatticeBoltzmannOutput.hpp"
#include "Level.hpp"
#include "glm/vec3.hpp"

using namespace glm;

class LatticeBoltzmann {
private:
    Level &level;

    // previous f_i field
    Array2D<FICell> fi_Old;

    // processed f_i field
    Array2D<FICell> fi_New;

    // internal f_i equilibrium field
    Array2D<FICell> fi_Eq;

    // quadrature weights for approximating equilibrium distribution
    int w [9] = {4/9, 1/9, 1/9, 1/9, 1/9, 1/36, 1/36, 1/36, 1/36};
    int cx [9] = {0, 1, 0, -1, 0 , 1, -1, -1, 1};
    int cy [9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
    int opp [9] = {0, 3, 4, 1, 2, 7, 8, 5, 6}; 

public:
    LatticeBoltzmann(Level &level)
        : level(level), fi_Old(level.width, level.height),
          fi_New(level.width, level.height), fi_Eq(level.width, level.height) {
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
		//set initial values
                for (int z = 0; z < 9; z++)
                {
                     fi_New.value(x,y)[z] = w[z]*1;
		     fi_Old.value(x,y)[z] = w[z]*1;
                }
            }
        }
    }

    void compute(const LatticeBoltzmannInput &input, LatticeBoltzmannOutput &output) {
        if (output.matrix.get() == nullptr) {
            output.matrix = std::make_unique<Array2D<vec3>>(level.width, level.height);
        }




//######################################## Collision ####################################################
for (int y = 1; y < level.height-1; ++y) {
        for (int x = 1; x < level.width-1; ++x) {
	    //check for boundary 
            if (input.matrix->value(x,y)[2]==0) {
                
                float rho = 0.0 ;
                float velx = 0.0 ;
                float vely = 0.0 ;
          
		//calculate density
		rho = fi_Old.value(x,y)[0] + fi_Old.value(x,y)[1] + fi_Old.value(x,y)[2] + fi_Old.value(x,y)[3] + fi_Old.value(x,y)[4] + fi_Old.value(x,y)[5] + fi_Old.value(x,y)[6] + fi_Old.value(x,y)[7] + fi_Old.value(x,y)[8];
                
                
//TODO later          density->get(x, y, vector::x) = rho ;
                const float rhoinv = 1.0 /rho;
                
		velx = ( (fi_Old.value(x,y)[5] + fi_Old.value(x,y)[1] + fi_Old.value(x,y)[8]) - (fi_Old.value(x,y)[6] + fi_Old.value(x,y)[3] + fi_Old.value(x,y)[7]) ) * rhoinv;
                
//TODO later   velocity->get(x, y, vector::x) = velx ;
                vely = ( (fi_Old.value(x,y)[6] + fi_Old.value(x,y)[2] + fi_Old.value(x,y)[5]) - (fi_Old.value(x,y)[7] + fi_Old.value(x,y)[4] + fi_Old.value(x,y)[8]) ) * rhoinv;
                
// TODO later              velocity->get(x, y, vector::y) = vely ;
                
                const float velxx = velx * velx ;
                const float velyy = vely * vely ;
                
//TODO need to set omega
float omega = 1.0;
                // For the center
                float feqC = w[0] * rho * (1.0 - 3.0 * (velxx + velyy) * 0.5 ) ;
                fi_Old.value(x,y)[0] +=  omega * (feqC - fi_Old.value(x,y)[0]) ;
                
//TODO later what is ax velocity of obsatcle
	float ax = 0;
	float ay = 0;
                // For the east distribution function
                float feqE = w[1] * rho * (1.0 + 3.0 * velx + 4.5 * velxx - 1.5 * (velxx + velyy))  ;
                fi_Old.value(x,y)[1] +=  omega * ( feqE - fi_Old.value(x,y)[1]) + 3.0 * w[1] * rho * ax ;
                
                //for the North East Direction
                float feqNE = w[5] * rho * (1.0 + 3.0*(velx + vely) + 4.5 * (velx+vely) * (velx + vely) - 1.5* (velxx+ velyy)) ;
                fi_Old.value(x,y)[5] += omega * (feqNE - fi_Old.value(x,y)[5]) + 3.0 * w[1] * rho * (ax + ay) ;
   
                // For the north direction
                float feqN = w[2] * rho * (1.0 + 3.0 * vely + 4.5 * velyy - 1.5 * (velxx + velyy) ) ;
                fi_Old.value(x,y)[2] += omega * (feqN - fi_Old.value(x,y)[2]) + 3.0 * w[1] * rho * ay;
                
                // For the North west direction
                float feqNW = w[6] * rho * (1.0 + 3.0 * (-velx + vely) + 4.5 * (-velx + vely) * (-velx + vely) - 1.5 * (velxx + velyy) ) ;
                fi_Old.value(x,y)[6] += omega * (feqNW - fi_Old.value(x,y)[6]) + 3.0 * w[1] * rho * (-ax + ay);
                
                // for the west direction
                float feqW  = w[3] * rho * (1.0 - 3.0 * velx + 4.5 * velxx - 1.5 * (velxx+ velyy)) ;
                fi_Old.value(x,y)[3] += omega * (feqW - fi_Old.value(x,y)[3])+ 3.0 * w[1] * rho * (-ax) ;
                
                // for the south west
                float feqSW = w[7] * rho * (1.0 -3.0 * (velx+ vely) + 4.5 * (velx+ vely) * (velx+ vely) - 1.5 * (velxx+ velyy) ) ;
                fi_Old.value(x,y)[7] += omega * (feqSW - fi_Old.value(x,y)[7])+ 3.0 * w[1] * rho * (-ax-ay) ;
                
                // for the South direction
                float feqS = w[4] * rho * (1.0 -3.0 * vely + 4.5 *velyy - 1.5 * (velxx + velyy) ) ;
                fi_Old.value(x,y)[4] += omega * (feqS  - fi_Old.value(x,y)[4]) + 3.0 * w[1] * rho * (-ay) ;
                
                // for the south east direction
                float feqSE = w[8] * rho * (1.0 + 3.0 * (velx - vely) + 4.5 * (velx-vely) * (velx-vely) - 1.5 * (velxx+ velyy) ) ;
                fi_Old.value(x,y)[8] += omega * (feqSE - fi_Old.value(x,y)[8]) + 3.0 * w[1] * rho * (ax-ay)  ;
		}
            }
        }
//#################################### End of Collision ##################################################

//#################################### Bounce Back Boundary #################################
	for (int y = 0; y < level.height; ++y) {
        	for (int x = 0; x < level.width; ++x) {
	   	 //check for boundary 
            	if (input.matrix->value(x,y)[2]==1) 
		{
			fi_New.value(x,y)[0]  = fi_Old.value(x,y)[0];
			if (x+1 >= level.width || input.matrix->value(x+1,y)[2] == 2){ //E
				fi_New.value(x,y)[1] = fi_Old.value(x,y)[opp[1]];
			} else 
			{
                		fi_New.value(x,y)[1]  = fi_Old.value(x-1,y)[1];
			}
			if (x+1 >= level.width || y+1 >= level.height || input.matrix->value(x+1,y+1)[2] == 2){ //NE
				fi_New.value(x,y)[5] = fi_Old.value(x,y)[opp[5]];
			} else
			{
                		fi_New.value(x,y)[5] = fi_Old.value(x-1,y-1)[5] ;
			}
			if (y+1 >= level.height || input.matrix->value(x,y+1)[2] == 2){ //N
				fi_New.value(x,y)[2] = fi_Old.value(x,y)[opp[2]];
			} else
			{
                		fi_New.value(x,y)[2]  = fi_Old.value(x,y-1)[2] ;
			}
			if (x-1 < 0 || y+1 >= level.height || input.matrix->value(x-1,y+1)[2] == 2){ //NW
				fi_New.value(x,y)[6] = fi_Old.value(x,y)[opp[6]];
			} else
			{
                		fi_New.value(x,y)[6] = fi_Old.value(x+1,y-1)[6] ;
			}
			if (x-1 < 0 || input.matrix->value(x-1,y)[2] == 2){ //W
				fi_New.value(x,y)[3] = fi_Old.value(x,y)[opp[3]];
			} else
			{
                		fi_New.value(x,y)[3]  = fi_Old.value(x+1,y)[3] ;
			}
			if (x-1 < 0 || y-1 < 0 || input.matrix->value(x-1,y-1)[2] == 2){ //SW
				fi_New.value(x,y)[7] = fi_Old.value(x,y)[opp[7]];
			} else
			{
                		fi_New.value(x,y)[7] = fi_Old.value(x+1,y+1)[7] ;
			}
			if ( y-1 < 0 || input.matrix->value(x,y-1)[2] == 2){ //S
				fi_New.value(x,y)[4] = fi_Old.value(x,y)[opp[4]];
			} else
			{
                		fi_New.value(x,y)[4]  = fi_Old.value(x,y+1)[4];
			}
             		if (x+1 >= level.width || y-1 < 0 || input.matrix->value(x+1,y-1)[2] == 2){ //SE
				fi_New.value(x,y)[8] = fi_Old.value(x,y)[opp[8]];
			} else
			{
                		fi_New.value(x,y)[8] = fi_Old.value(x-1,y+1)[8] ;
			}
                	
                }
        	}
   	}

//#################################### End of Bounce Back ##############################################

//#################################### Streaming ############################################
        for (int y = 1; y < level.height-1; ++y) {
        	for (int x = 1; x < level.width-1; ++x) {
	   	 //check for boundary 
            	if (input.matrix->value(x,y)[2]==0) {
               		fi_New.value(x,y)[0]  = fi_Old.value(x,y)[0] ;
                	fi_New.value(x,y)[1]  = fi_Old.value(x-1,y)[1];
                	fi_New.value(x,y)[5] = fi_Old.value(x-1,y-1)[5] ;
                	fi_New.value(x,y)[2]  = fi_Old.value(x,y-1)[2] ;
                	fi_New.value(x,y)[6] = fi_Old.value(x+1,y-1)[6] ;
                	fi_New.value(x,y)[3]  = fi_Old.value(x+1,y)[3] ;
                	fi_New.value(x,y)[7] = fi_Old.value(x+1,y+1)[7] ;
                	fi_New.value(x,y)[4]  = fi_Old.value(x,y+1)[4];
                	fi_New.value(x,y)[8] = fi_Old.value(x-1,y+1)[8] ;
                      	}
        	}
   	}
// now fi_New contains the streamed and collided values
//#################################### End of Streaming ##########################################

        // TODO Consider Boundary in Col & STream (0 = fluid, 1 = boundary, 2 =
        // inflow, 3 = outflow)

//#################################### Output #######################################
        // Calculate macroscopic quantities for the output
        for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
                output.matrix->value(x, y)[2] = 0;
                for (int i = 0; i < 9; i++) {
                    output.matrix->value(x, y)[2] += fi_New.value(x, y)[i]; // density
                }
                output.matrix->value(x, y)[0] = fi_New.value(x, y)[1] - fi_New.value(x, y)[3] +
                                                fi_New.value(x, y)[5] - fi_New.value(x, y)[6] -
                                                fi_New.value(x, y)[7] +
                                                fi_New.value(x, y)[8]; // x Velocity
                output.matrix->value(x, y)[1] = fi_New.value(x, y)[2] - fi_New.value(x, y)[4] +
                                                fi_New.value(x, y)[5] + fi_New.value(x, y)[6] -
                                                fi_New.value(x, y)[7] -
                                                fi_New.value(x, y)[8]; // y Velocity
            }
        }

        // Set fi_old = fi_new
	for (int y = 0; y < level.height; y++) {
            for (int x = 0; x < level.width; x++) {
	        for (int z = 0; z < 9; ++z) {
			fi_Old.value(x,y)[z]=fi_New.value(x,y)[z];
		}	
	    }
	}
    }
};

#endif
