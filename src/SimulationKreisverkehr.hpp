#include "StreetMap.hpp"

using namespace std;

class SimulationKreisverkehr {
	public:
		SimulationKreisverkehr( default_random_engine &randomEngine ) :
			randomEngine( randomEngine ) {
		}
		
		void simulateStep( StreetMap &streetMap ) {
			addCars();
			
			accelerate( streetMap );
			changeLanes();
			dally();
			move();
		}
	private:
		default_random_engine &randomEngine;
		
		void accellerate( StreetMap &streetMap ) {
		}
};
