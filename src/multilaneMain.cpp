#include <getopt.h>
#include "vehicle.hpp"
#include "multilaneRoad.hpp"
#include "multilaneSimulation.hpp"

using namespace std;

void printHelp( int argc, char **argv ) {
	cout << "SYNOPSIS" << endl;
	cout << "\t" << argv[0] << " [-r] [-iWHlcdDsS]" << endl;
	cout << "\t" << argv[0] << " -x [-ilcdDsS]" << endl;
	cout << "\t" << argv[0] << " -h | --help" << endl;
	
	cout << "OPTIONS" << endl;
	cout << "\t" << "-i #, --iterations #" << endl;
	cout << "\t\t" << "Run simulation for # iterations" << endl;
	
	cout << "\t" << "-w, --wrap-around" << endl;
	cout << "\t\t" << "Use cyclic boundary conditions (default is open boundary conditions)" << endl;
	cout << "\t" << "-p, --pre-fill-road" << endl;
	cout << "\t\t" << "Place vehicles on the road during initialization" << endl;
	cout << "\t" << "-e, --equally-spaced" << endl;
	cout << "\t\t" << "Place initial vehicles (-p) at equal distance" << endl;
	cout << "\t" << "-x #, --street-length #" << endl;
	cout << "\t\t" << "Set length of the street to # cells" << endl;
	cout << "\t" << "-y #, --lanes #" << endl;
	cout << "\t\t" << "Set number of lanes to #" << endl;
	cout << "\t" << "-t #, --traffic-density #" << endl;
	cout << "\t\t" << "Aim for a traffic density of #" << endl;
	
	cout << "\t" << "-d #, --min-dally-factor #" << endl;
	cout << "\t\t" << "Set minimum dally factor to #" << endl;
	cout << "\t" << "-D #, --max-dally-factor #" << endl;
	cout << "\t\t" << "Set maximum dally factor to #" << endl;
	cout << "\t" << "-r #, --lambda-risk-factor-l2r #" << endl;
	cout << "\t\t" << "Set risk factor for changing from left to right lane to #" << endl;
	cout << "\t" << "-R #, --lambda-risk-factor-r2l #" << endl;
	cout << "\t\t" << "Set risk factor for changing from right to left lane to #" << endl;
	cout << "\t" << "-s #, --speed-mean #" << endl;
	cout << "\t\t" << "Set mean of the max-speed normal distribution to #" << endl;
	cout << "\t" << "-S #, --speed-std #" << endl;
	cout << "\t\t" << "Set standard deviation of the max-speed normal distribution to #" << endl;
	
	cout << "\t" << "-h, --help" << endl;
	cout << "\t\t" << "Display this message." << endl;
}

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
		{"help", no_argument, nullptr, 'h'},
		
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
	while( ( option = getopt_long( argc, argv, "hi:wpex:y:t:d:D:r:R:s:S:", options, nullptr ) )
	        != -1 ) {
		switch( option ) {
			case 'h':
				printHelp( argc, argv );
				exit( EXIT_SUCCESS );
				break;
				
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
	
	if( minDallyFactor > maxDallyFactor || minDallyFactor < 0. || maxDallyFactor < 0. ) {
		cerr << "Please choose values between 0 and 1 for the minimal and maximal dally factor." << endl
		     <<	"The maximal dally factor must not be smaller than the minimal dally factor!" << endl;
	}
	
	MultilaneSimulation simulation( minDallyFactor, maxDallyFactor,
	                                lambdaRiskFactorL2R, lambdaRiskFactorR2L, maxSpeedMean,
	                                maxSpeedStd );
	simulation.initialize( streetLength, laneCount, trafficDensity, wrapAround, fillRoad,
	                       equallySpaced );
	                       
	simulation.simulate( iterations );
	
	return EXIT_SUCCESS;
}
