#pragma once

#include <deque>
#include "StreetMap.hpp"
#include "VisualizationKreisverkehr.hpp"

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
			
			populateMap();
			
			//visualization = new VisualizationKreisverkehr(streetMap.getContents().size(), streetMap.getContents()[0].size());
			//visualization->appendRoundabout(streetMap);
			
			for( long i = 0; i < iterations; ++i ) {
				this->streetMap->visualize();
				//visualization->appendRoundabout(streetMap);
				simulateStep();
			}
			
			//visualization->save();
			
			this->streetMap = nullptr;
			this->trafficDensity = 0;
			
			//delete visualization;
		}
		
		void simulateStep() {
			streetMap->drawDestinations();
			streetMap->clearMarks();
			
//			addCars();
//			accelerate();
			checkDistances();
//			dally();
			//move();
			
			streetMap->clearMarks();
		}
	private:
		default_random_engine &randomEngine;
		uniform_real_distribution<double> dallyFactorDistribution;
		exponential_distribution<double> riskFactorDistribution;
		normal_distribution<double> maxSpeedDistribution;
		
		StreetMap *streetMap;
		double trafficDensity;
		
		void populateMap() {
			vector< vector<StreetSegment> > &contents = streetMap->getContents();
			bernoulli_distribution carPlacementDistribution( trafficDensity );
			
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					if( ! contents[x][y].isDummy() ) {
						if( carPlacementDistribution( randomEngine ) ) {
							contents[x][y].v = new Vehicle( randomEngine, dallyFactorDistribution,
							                                riskFactorDistribution,
							                                maxSpeedDistribution );
						}
					}
				}
			}
		}
		
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
		
		void checkDistances() {
			vector< vector<StreetSegment> > &contents = streetMap->getContents();
			deque<StreetSegment*> carSegments;
			
//			Get segments that contain cars, and mark inhabited segments
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					if( contents[x][y].v != nullptr ) {
						contents[x][y].mark = contents[x][y].v;
						carSegments.push_back( &( contents[x][y] ) );
					}
				}
			}
			
//			Place markings along each Vehicle's path. Overwrite other paths according to right of way
			for( auto startSegment = carSegments.begin(); startSegment < carSegments.end();
			        ++startSegment ) {
				if( ( *startSegment )->isSink() ) {
					continue;
				}
				
				StreetSegment *previousSegment = nullptr;
				StreetSegment *nextSegment = *startSegment;
				
				for( long step = 0; step < ( *startSegment )->v->currentSpeed; ++step ) {
					previousSegment = nextSegment;
					nextSegment = nextSegment->destinations[nextSegment->nextDestination];
					if( nextSegment->mark != nullptr ) { //Someone was here before
						if( nextSegment->predecessors[0] != previousSegment ) { // No right of way
							break;
						}
					}
					nextSegment->mark = ( *startSegment )->v;
					if( nextSegment->isSink() ) {
						break;
					}
				}
			}
			
//			Determine max speed according to markings
//			TODO take maxSpeed of each segment into account
			for( auto startSegment = carSegments.begin(); startSegment < carSegments.end();
			        ++startSegment ) {
				if( !( *startSegment )->isSink() ) {
					StreetSegment *nextSegment =
					    ( *startSegment )->destinations[( *startSegment )->nextDestination];
					long stepsTaken = 0;
					while( nextSegment->mark == ( *startSegment )->v ) {
						++stepsTaken;
						if( nextSegment->isSink() ) {
							stepsTaken = ( *startSegment )->v->currentSpeed;
							break;
						}
						nextSegment = nextSegment->destinations[nextSegment->nextDestination];
					}
					
					( *startSegment )->v->currentSpeed = stepsTaken;
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
		
		VisualizationKreisverkehr *visualization;
};
