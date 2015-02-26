#include "StreetMap.hpp"

using namespace std;

class SimulationKreisverkehr {
	public:
		SimulationKreisverkehr( default_random_engine &randomEngine,
		                        uniform_real_distribution<double> &dallyFactorDistribution,
		                        exponential_distribution<double> &riskFactorDistribution,
		                        normal_distribution<double> &maxSpeedDistribution ) :
			randomEngine( randomEngine ),
			dallyFactorDistribution( dallyFactorDistribution ),
			riskFactorDistribution( riskFactorDistribution ),
			maxSpeedDistribution( maxSpeedDistribution ) {
			streetMap = nullptr;
		}
		
		void simulate( StreetMap &streetMap, double trafficDensity, long iterations ) {
			this->streetMap = &streetMap;
			this->trafficDensity = trafficDensity;
			
			for( long i = 0; i < iterations; ++i ) {
				this->streetMap->visualize();
				simulateStep();
			}
			
			this->streetMap = nullptr;
			this->trafficDensity = 0;
		}
		
		void simulateStep() {
			streetMap->drawDestinations();
			
			addCars();
			
			accelerate();
			//checkDistances();
			dally();
			//move();
		}
	private:
		default_random_engine &randomEngine;
		uniform_real_distribution<double> dallyFactorDistribution;
		exponential_distribution<double> riskFactorDistribution;
		normal_distribution<double> maxSpeedDistribution;
		
		StreetMap *streetMap;
		double trafficDensity;
		
		void addCars() {
			set<StreetSegment*> &sources = streetMap->getSources();
			for( auto s = sources.begin(); s != sources.end(); ++s ) {
				double currentDensity = streetMap->computeDensity();
				if( currentDensity < trafficDensity && ( *s )->v == nullptr ) {
					( *s )->v = new Vehicle( randomEngine, dallyFactorDistribution,
					                         riskFactorDistribution, maxSpeedDistribution );
				}
			}
		}
		
		void accelerate() {
			vector< vector<StreetSegment> > &contents = streetMap->getContents();
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					if( contents[x][y].v != nullptr ) {
						contents[x][y].v->accelerate();
					}
				}
			}
		}
		
		void dally() {
			vector< vector<StreetSegment> > &contents = streetMap->getContents();
			uniform_real_distribution<double> uniform01distribution( 0.0, 1.0 );
			
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					Vehicle *v = contents[x][y].v;
					if( v != nullptr ) {
						if( uniform01distribution( randomEngine ) < v->dallyFactor ) {
							contents[x][y].v->accelerate( -1 );
						}
					}
				}
			}
		}
};
