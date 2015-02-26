#include "StreetMap.hpp"
#include "SimulationKreisverkehr.hpp"

using namespace std;

int main( int argc, char **argv ) {
	double minDallyFactor = 0.;
	double maxDallyFactor = 0.3;
	double lambdaRiskFactor = 5.;
	double maxSpeedMean = 5.;
	double maxSpeedStd = 1.2;
	
	default_random_engine randomEngine( chrono::system_clock::now().time_since_epoch().count() );
	
	uniform_real_distribution<double> dallyFactorDistribution( minDallyFactor, maxDallyFactor );
	exponential_distribution<double> riskFactorDistribution( lambdaRiskFactor );
	normal_distribution<double> maxSpeedDistribution( maxSpeedMean, maxSpeedStd );
	
	long dimX = 70;
	long dimY = 70;
	double trafficDensity = 0.3;
	
	StreetMap streetMap( dimX, dimY, trafficDensity, randomEngine, dallyFactorDistribution,
	                     riskFactorDistribution, maxSpeedDistribution );
	SimulationKreisverkehr simulation( randomEngine );
	return EXIT_SUCCESS;
}
