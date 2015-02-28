#include <getopt.h>
#include "Vehicle.hpp"
#include "Road.hpp"
#include "SimulationMehrspurig.hpp"

using namespace std;

int main( int argc, char **argv ) {
	long iterations = 500;
	
	long streetLength = 300;
	long laneCount = 4;
	double trafficDensity = 0.1;
	
	double minDallyFactor = 0.;
	double maxDallyFactor = 0.3;
	double lambdaRiskFactor = 5.;
	double maxSpeedMean = 5.;
	double maxSpeedStd = 1.2;
	
	struct option options[] = {
		{"iterations", required_argument, nullptr, 'i'},
		
		{"street-length", required_argument, nullptr, 'x'},
		{"lanes", required_argument, nullptr, 'y'},
		{"traffic-density", required_argument, nullptr, 't'},
		
		{"min-dally-factor", required_argument, nullptr, 'd'},
		{"max-dally-factor", required_argument, nullptr, 'D'},
		{"lambda-risk-factor", required_argument, nullptr, 'r'},
		{"speed-mean", required_argument, nullptr, 's'},
		{"speed-std", required_argument, nullptr, 'S'},
		
		{0, 0, nullptr, 0}
	};
	
	char option;
	while( ( option = getopt_long( argc, argv, "i:x:y:t:d:D:r:s:S:", options, nullptr ) ) != -1 ) {
		switch( option ) {
			case 'i':
				iterations = atoi( optarg );
				break;
				
			case 'x':
				streetLength = atoi( optarg );
				break;
			case 'y':
				laneCount = atoi( optarg );
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
			case 'r':
				lambdaRiskFactor = atof( optarg );
				break;
			case 's':
				maxSpeedMean = atof( optarg );
				break;
			case 'S':
				maxSpeedStd = atof( optarg );
				break;
				
			case '?':
				if( optopt == 'i' || optopt == 'x' || optopt == 'y' || optopt == 't' ||
				        optopt == 'd' || optopt == 'D' || optopt == 'r' || optopt == 's' ||
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
	
	SimulationMehrspurig simulation( minDallyFactor, maxDallyFactor, lambdaRiskFactor, maxSpeedMean,
	                                 maxSpeedStd );
	simulation.initialize( streetLength, laneCount, trafficDensity );
	
	simulation.simulate( iterations );
}
