#ifndef CSTAGE_RIGIDBODYSIMULATION_HPP_
#define CSTAGE_RIGIDBODYSIMULATION_HPP_

#include "CParameters.hpp"
#include "CPipelineStage.hpp"
#include "CDataRigidBody.hpp"

class CStage_RigidBodySimulation : public CPipelineStage {
private:
	/**
	 * global parameters
	 */
	CParameters &cParameters;

	// Input comes from LBM sim. Grid with Velocities and Densities
	/**
	 * input velocity and density field
	 * TODO: Discuss with LBM guys about how data is stored
	 */
	CDataArray2D<float,3> input_cDataArray2D_vel_den;

	/**
	 * output velocity and flag field for LBM
	 */
	CDataArray2D<float,3> output_cDataArray2D_vel_flag;

	/**
	 * output list of rigid bodies for viz
	 */
	CDataRigidBodyList output_cDataRigidBody;

public:
	// TODO: Maybe the parameters are not required. Check later.
	CStage_RigidBodySimulation(CParameters &i_cParameters) :
	 	CPipelineStage("RigidBodySimulation"),
		cParameters(i_cParameters) {}

	/**
	 * manually triggered pushing into the pipeline
	 TODO: Make sure that the other classes know what to look
	 for when pipeline_process_input is called
	 */
	void pipeline_push() {
		// The following is for LBM
		CPipelineStage::pipeline_push((CPipelinePacket&)output_cDataArray2D_vel_flag);

		// The following is for visualization
		CPipelineStage::pipeline_push((CPipelinePacket&)output_cDataRigidBody);
	}

	// Update method. Here all the magic happens. Collisions. Force computations. etc.
	void simulation_timestep() {
		// magic
		pipeline_push();
	}

	void main_loop_callback() {
		simulation_timestep();
	}

	// Obtain data from LBM
	void pipeline_process_input(CPipelinePacket& i_cPipelinePacket) {
		if (i_cPipelinePacket.type_info_name == typeid(CDataArray2D<float,3>)) {
			// Process LBM data
		}

		// unpack data
		CDataArray2D<float,3> *input = i_cPipelinePacket.getPayload<CDataArray2D<float,3>>();

		// Process the input data
		input_cDataArray2D_vel_den.resize(input->width, input->height);
		input_cDataArray2D_vel_den.loadData(input->data);
	}

	virtual ~CStage_RigidBodySimulation ();
};
#endif // CSTAGE_RIGIDBODYSIMULATION_HPP_
