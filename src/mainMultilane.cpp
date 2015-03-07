#include <getopt.h>
#include "Vehicle.hpp"
#include "Road.hpp"
#include "SimulationMultilane.hpp"

using namespace std;

int main( int argc, char **argv ) {
	long iterations = 500;
	
	long streetLength = 300;
	long laneCount = 4;
	double trafficDensity = 0.1;
	
	double minDallyFactor = 0.;
	double maxDallyFactor = 0.3;
	double lambdaRiskFactorL2R = 12.;
	double lambdaRiskFactorR2L = 8.;
	double maxSpeedMean = 5.;
	double maxSpeedStd = 1.2;
	
	bool wrapAround = false;
	bool fillRoad = false;
	bool equallySpaced = false;
	
	struct option options[] = {
		{"iterations", required_argument, nullptr, 'i'},
		
		{"wrap-around", no_argument, nullptr, 'w'},
		{"pre-fill-road", no_argument, nullptr, 'p'},
		{"equally-spaced", no_argument, nullptr, 'e'},
		{"street-length", required_argument, nullptr, 'x'},
		{"lanes", required_argument, nullptr, 'y'},
		{"traffic-density", required_argument, nullptr, 't'},
		
		{"min-dally-factor", required_argument, nullptr, 'd'},
		{"max-dally-factor", required_argument, nullptr, 'D'},
		{"lambda-risk-factor-l2r", required_argument, nullptr, 'r'},
		{"lambda-risk-factor-r2l", required_argument, nullptr, 'R'},
		{"speed-mean", required_argument, nullptr, 's'},
		{"speed-std", required_argument, nullptr, 'S'},
		
		{0, 0, nullptr, 0}
	};
	
	char option;
	while( ( option = getopt_long( argc, argv, "i:wpex:y:t:d:D:r:R:s:S:", options, nullptr ) )
	        != -1 ) {
		switch( option ) {
			case 'i':
				iterations = atoi( optarg );
				break;
				
			case 'p':
				fillRoad = true;
				break;
			case 'e':
				equallySpaced = true;
				break;
			case 'w':
				wrapAround = true;
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
				lambdaRiskFactorL2R = atof( optarg );
				break;
			case 'R':
				lambdaRiskFactorR2L = atof( optarg );
				break;
			case 's':
				maxSpeedMean = atof( optarg );
				break;
			case 'S':
				maxSpeedStd = atof( optarg );
				break;
				
			case '?':
				if( optopt == 'i' || optopt == 'x' || optopt == 'y' || optopt == 't' ||
				        optopt == 'd' || optopt == 'D' || optopt == 'r' || optopt == 'R' ||
				        optopt == 's' || optopt == 'S' ) {
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
	
	if (minDallyFactor > maxDallyFactor || minDallyFactor < 0. || maxDallyFactor < 0.) {
				cerr << "Please choose values between 0 and 1 for the minimal and maximal dally factor." << endl
					 <<	"The maximal dally factor must not be smaller than the minimal dally factor!" << endl;
	}

	SimulationMultilane simulation( minDallyFactor, maxDallyFactor,
	                                lambdaRiskFactorL2R, lambdaRiskFactorR2L, maxSpeedMean,
	                                maxSpeedStd );
	simulation.initialize( streetLength, laneCount, trafficDensity, wrapAround, fillRoad, equallySpaced );
	
	simulation.simulate( iterations );
	
	return EXIT_SUCCESS;
}
