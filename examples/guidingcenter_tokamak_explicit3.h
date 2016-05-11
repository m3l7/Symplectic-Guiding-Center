#ifndef CONFIG_H
#define CONFIG_H

#include "utils/configInterface.h"
#include "utils/particleUtils.h"

using namespace std;
using namespace Particles;

namespace Config{

	const int DIM = 4;

	class Config : public ConfigInterface{
		public:
			Config() : ConfigInterface(){
				magneticField = "Tokamak";
				system = "GuidingCenter";
				integrator = "SymplecticExplicit3";
				auxiliary_integrator = "RK4";
				init_steps = 1000;

				h = 100; //timestep
				max_t = 1.E5; //number of timesteps to compute
				time_offset = 0; //timestep when to begin simulation
				exit_on_error = false; //exit if the error is too high
				error_threshold = 0.1; //error threshold to exit simulation if above
				orbit_normalize = 50; //timestep normalization

				z.q0 << 0.050000000000000, 0.00000000000000 ,0.000000000000000 ,0.000390000000000; //initial condition
				initialization_type = INIT_LAGRANGIAN; // {INIT_HAMILTONIAN, INIT_LAGRANGIAN, INIT_MANUAL_POSITION_MOMENTUM, INIT_MANUAL_MULTISTEP}
				B0 = 1.; // magnetic field B0
				R0 = 1.; // major radius
				kt = 1.; // toroidal field correction
				q = 1.; // safety factor

				outFile = "out/out.txt"; //output file

				print_precision = 15; // output digit precision
				print_timestep_mult = 10000; // print to screen every n timesteps
				file_timestep_mult = 8; //print to file every n timesteps
				print_timestep_offset = 1; //print to screen initial offset

				debug_level = "info"; //set to info(default)/debug/trace

			};
			~Config(){};

			PositionMomentumTwoPoints<DIM> z;

			//guiding center specific
			double B0,R0,kt,q;
	};
}

#endif