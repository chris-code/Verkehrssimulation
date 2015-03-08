#include <getopt.h>
#include <iostream>
#include "streetmapRoad.hpp"
#include "streetmapSimulation.hpp"

using namespace std;

void printHelp( int argc, char **argv ) {
	cout << "SYNOPSIS" << endl;
	cout << "\t" << argv[0] << " [-r] [-iWHlcdDsS]" << endl;
	cout << "\t" << argv[0] << " -x [-ilcdDsS]" << endl;
	cout << "\t" << argv[0] << " -h | --help" << endl;
	
	cout << "OPTIONS" << endl;
	cout << "\t" << "-i #, --iterations #" << endl;
	cout << "\t\t" << "Run simulation for # iterations" << endl;
	
	cout << "\t" << "-r, --roundabout (default)" << endl;
	cout << "\t\t" << "Simulate a roundabout (instead of interchange (-x))" << endl;
	cout << "\t" << "-x, --interchange" << endl;
	cout << "\t\t" << "Simulate an interchange (instead of the default roundabout (-r))" << endl;
	cout << "\t" << "-W #, --width #" << endl;
	cout << "\t\t" << "Roundabout width should be # cells" << endl;
	cout << "\t" << "-H #, --height #" << endl;
	cout << "\t\t" << "Roundabout height should be # cells" << endl;
	cout << "\t" << "-l #, --drive-up-length #" << endl;
	cout << "\t\t" << "Drive-up lengths should be # cells (both roundabout and interchange)" << endl;
	
	cout << "\t" << "-c #, --car-generation-rate #" << endl;
	cout << "\t\t" << "Set car generation rate of each source to #" << endl;
	
	cout << "\t" << "-d #, --min-dally-factor #" << endl;
	cout << "\t\t" << "Set minimum dally factor to #" << endl;
	cout << "\t" << "-D #, --max-dally-factor #" << endl;
	cout << "\t\t" << "Set maximum dally factor to #" << endl;
	cout << "\t" << "-s #, --speed-mean #" << endl;
	cout << "\t\t" << "Set mean of the max-speed normal distribution to #" << endl;
	cout << "\t" << "-S #, --speed-std #" << endl;
	cout << "\t\t" << "Set standard deviation of the max-speed normal distribution to #" << endl;
	
	cout << "\t" << "-h, --help" << endl;
	cout << "\t\t" << "Display this message." << endl;
}

int main( int argc, char **argv ) {
	long iterations = 500;
	
	bool simulateRoundabout = true;
	long roundaboutWidth = 8;
	long roundaboutHeight = 6;
	long driveUpLength = 20;
	double carGenerationRate = 0.2;
	
	double minDallyFactor = 0.;
	double maxDallyFactor = 0.3;
	double lambdaRiskFactorL2R = 12.; // Not used in this program, but required by Vehicle.hpp
	double lambdaRiskFactorR2L = 8.; // Not used in this program, but required by Vehicle.hpp
	double maxSpeedMean = 5.;
	double maxSpeedStd = 1.2;
	
	struct option options[] = {
		{"help", no_argument, nullptr, 'h'},
		
		{"iterations", required_argument, nullptr, 'i'},
		
		{"roundabout", required_argument, nullptr, 'r'},
		{"interchange", required_argument, nullptr, 'x'},
		{"width", required_argument, nullptr, 'W'},
		{"height", required_argument, nullptr, 'H'},
		{"drive-up-length", required_argument, nullptr, 'l'},
		{"car-generation-rate", required_argument, nullptr, 'c'},
		
		{"min-dally-factor", required_argument, nullptr, 'd'},
		{"max-dally-factor", required_argument, nullptr, 'D'},
		{"speed-mean", required_argument, nullptr, 's'},
		{"speed-std", required_argument, nullptr, 'S'},
		
		{0, 0, nullptr, 0}
	};
	
	char option;
	while( ( option = getopt_long( argc, argv, "hi:rxW:H:l:c:d:D:s:S:", options, nullptr ) ) != -1 ) {
		switch( option ) {
			case 'h':
				printHelp( argc, argv );
				exit( EXIT_SUCCESS );
				break;
				
			case 'r':
				simulateRoundabout = true;
				break;
			case 'x':
				simulateRoundabout = false;
				break;
			case 'i':
				iterations = atoi( optarg );
				break;
			case 'W':
				roundaboutWidth = atoi( optarg );
				break;
			case 'H':
				roundaboutHeight = atoi( optarg );
				break;
			case 'l':
				driveUpLength = atoi( optarg );
				break;
			case 'c':
				carGenerationRate = atof( optarg );
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
				if( optopt == 'i' || optopt == 'W' || optopt == 'H' || optopt == 'l' ||
				        optopt == 'c' || optopt == 'd' || optopt == 'D' || optopt == 's' ||
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
	exponential_distribution<double> riskFactorDistributionL2R( lambdaRiskFactorL2R );
	exponential_distribution<double> riskFactorDistributionR2L( lambdaRiskFactorR2L );
	normal_distribution<double> maxSpeedDistribution( maxSpeedMean, maxSpeedStd );
	
	if( simulateRoundabout ) {
		StreetMap streetMap( roundaboutWidth, roundaboutHeight, driveUpLength, carGenerationRate,
		                     randomEngine );
		SimulationRoundabout simulation( randomEngine, dallyFactorDistribution,
		                                 riskFactorDistributionL2R, riskFactorDistributionR2L,
		                                 maxSpeedDistribution );
		simulation.simulate( streetMap, carGenerationRate, iterations );
	} else {
		StreetMap streetMap( randomEngine, driveUpLength, carGenerationRate );
		SimulationRoundabout simulation( randomEngine, dallyFactorDistribution,
		                                 riskFactorDistributionL2R, riskFactorDistributionR2L,
		                                 maxSpeedDistribution );
		simulation.simulate( streetMap, carGenerationRate, iterations );
	}
	
	return EXIT_SUCCESS;
}
