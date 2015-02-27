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
	
	long roundaboutWidth = 8;
	long roundaboutHeight = 6;
	long driveUpLength = 20;
	double trafficDensity = 0.1;
	
	long iterations = 50;
	
	StreetMap streetMap( roundaboutWidth, roundaboutHeight, driveUpLength, trafficDensity,
	                     randomEngine );
	SimulationKreisverkehr simulation( randomEngine, dallyFactorDistribution, riskFactorDistribution,
	                                   maxSpeedDistribution );
	simulation.simulate( streetMap, trafficDensity, iterations );
	
	return EXIT_SUCCESS;
}
