#include <getopt.h>
#include "StreetMap.hpp"
#include "SimulationKreisverkehr.hpp"

using namespace std;

int main( int argc, char **argv ) {
	long iterations = 50;
	
	long roundaboutWidth = 8;
	long roundaboutHeight = 6;
	long driveUpLength = 20;
	double trafficDensity = 0.1;
	
	double minDallyFactor = 0.;
	double maxDallyFactor = 0.3;
	double lambdaRiskFactor = 5.; // Not used in this program, but required by Vehicle.hpp
	double maxSpeedMean = 5.;
	double maxSpeedStd = 1.2;
	
	struct option options[] = {
		{"iterations", required_argument, nullptr, 'i'},
		
		{"width", required_argument, nullptr, 'w'},
		{"height", required_argument, nullptr, 'h'},
		{"drive-up-length", required_argument, nullptr, 'l'},
		{"traffic-density", required_argument, nullptr, 't'},
		
		{"min-dally-factor", required_argument, nullptr, 'd'},
		{"max-dally-factor", required_argument, nullptr, 'D'},
		{"speed-mean", required_argument, nullptr, 's'},
		{"speed-std", required_argument, nullptr, 'S'},
		
		{0, 0, nullptr, 0}
	};
	
	char option;
	while( ( option = getopt_long( argc, argv, "i:w:h:l:t:d:D:s:S:", options, nullptr ) ) != -1 ) {
		switch( option ) {
			case 'i':
				iterations = atoi( optarg );
				break;
			case 'w':
				roundaboutWidth = atoi( optarg );
				break;
			case 'h':
				roundaboutHeight = atoi( optarg );
				break;
			case 'l':
				driveUpLength = atoi( optarg );
				break;
			case 't':
				trafficDensity = atof( optarg );
				break;
				
			case 'd':
				minDallyFactor = atof( optarg );
				break;
			case 'D':
				maxDallyFactor = atof( optarg );
				break;
			case 's':
				maxSpeedMean = atof( optarg );
				break;
			case 'S':
				maxSpeedStd = atof( optarg );
				break;
				
			case '?':
				if( optopt == 'i' || optopt == 'w' || optopt == 'h' || optopt == 'l' ||
				        optopt == 't' || optopt == 'd' || optopt == 'D' || optopt == 's' ||
				        optopt == 'S' ) {
					cerr << "Option -%" << optopt << " requires an argument." << endl;
				} else {
					cerr << "Unknown option " << optopt << endl;
				}
				break;
			default:
				cerr << "Error in parameter handling: unknown case" << endl;
				exit( EXIT_FAILURE );
		}
	}
	
	default_random_engine randomEngine( chrono::system_clock::now().time_since_epoch().count() );
	uniform_real_distribution<double> dallyFactorDistribution( minDallyFactor, maxDallyFactor );
	exponential_distribution<double> riskFactorDistribution( lambdaRiskFactor );
	normal_distribution<double> maxSpeedDistribution( maxSpeedMean, maxSpeedStd );
	
	StreetMap streetMap( roundaboutWidth, roundaboutHeight, driveUpLength, trafficDensity,
	                     randomEngine );
	SimulationKreisverkehr simulation( randomEngine, dallyFactorDistribution, riskFactorDistribution,
	                                   maxSpeedDistribution );
	simulation.simulate( streetMap, trafficDensity, iterations );
	
	return EXIT_SUCCESS;
}
