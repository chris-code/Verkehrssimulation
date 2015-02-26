#include "Vehicle.hpp"
#include "Road.hpp"
#include "SimulationMehrspurig.hpp"

using namespace std;

int main() {
	double minDallyFactor = 0.;
	double maxDallyFactor = 0.3;
	double lambdaRiskFactor = 5.;
	double maxSpeedMean = 5.;
	double maxSpeedStd = 1.2;
	
	SimulationMehrspurig simulation( minDallyFactor, maxDallyFactor, lambdaRiskFactor, maxSpeedMean,
	                                 maxSpeedStd );
	                                 
	long streetLength = 300;
	long laneCount = 4;
	double carDensity = 0.1;
	
	simulation.initialize( streetLength, laneCount, carDensity );
	
	simulation.simulate( 500 );
}
