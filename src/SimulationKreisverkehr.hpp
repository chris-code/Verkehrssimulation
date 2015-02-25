#include "StreetMap.hpp"

using namespace std;

class SimulationKreisverkehr {
	public:
		SimulationKreisverkehr( long dimX, long dimY ) :
			randomEngine( chrono::system_clock::now().time_since_epoch().count() ),
			streetMap( dimX, dimY, randomEngine ) {
		}
	private:
		default_random_engine randomEngine;
		
		StreetMap streetMap;
};
