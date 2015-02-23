#include "Vehicle.hpp"
#include "Road.hpp"
#include "Simulation.hpp"


using namespace std;

int main() {
	double minDallyFactor = 0.;
	double maxDallyFactor = 0.4;
	double lambdaRiskFactor = 5.;
	double maxSpeedMean = 5.;
	double maxSpeedStd = 1.2;

	Simulation simulation(minDallyFactor, maxDallyFactor, lambdaRiskFactor, maxSpeedMean, maxSpeedStd);

	long streetLength = 300;
	long laneCount = 2;
	double carDensity = 0.3;

	simulation.initialize(streetLength, laneCount, carDensity);

	for (long i = 0; i < 500; ++i)
		simulation.update();
}
