#include "StreetMap.hpp"

using namespace std;

class SimulationKreisverkehr {
	public:
		SimulationKreisverkehr( default_random_engine &randomEngine ) :
			randomEngine( randomEngine ) {
		}
	private:
		default_random_engine &randomEngine;
};
