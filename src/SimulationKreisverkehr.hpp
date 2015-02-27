#pragma once

#include <deque>
#include "MessageException.hpp"
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
			
			VisualizationKreisverkehr vis(streetMap.getContents().size(), streetMap.getContents()[0].size());
			vis.appendRoundabout(streetMap);

			for( long i = 0; i < iterations; ++i ) {
				this->streetMap->visualize();
				simulateStep();
				vis.appendRoundabout(streetMap);
			}
			
			vis.save();

			this->streetMap = nullptr;
			this->trafficDensity = 0;
		}
		
	private:
		default_random_engine &randomEngine;
		uniform_real_distribution<double> dallyFactorDistribution;
		exponential_distribution<double> riskFactorDistribution;
		normal_distribution<double> maxSpeedDistribution;
		
		StreetMap *streetMap;
		double trafficDensity;
		
		void simulateStep() {
			streetMap->drawDestinations();
			streetMap->clearMarks();
			
			addCars();
			accelerate();
			checkDistances();
			dally();
			move();
			
			streetMap->clearMarks();
		}
		
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
			deque<StreetSegment*> carSegments; // Segments that contain a vehiclex
			
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
				
				for( long remainingSteps = ( *startSegment )->v->currentSpeed; remainingSteps > 0;
				        --remainingSteps ) {
					previousSegment = nextSegment;
					nextSegment = nextSegment->destinations[nextSegment->nextDestination];
					if( nextSegment->v != nullptr ) { // We ran into a car
						break;
					}
					if( nextSegment->mark != nullptr ) { // No vehicle, but someone was here before
						if( nextSegment->predecessors[0] != previousSegment ) { // No right of way
							break;
						}
					}
					nextSegment->mark = ( *startSegment )->v;
					if( nextSegment->isSink() ) {
						break;
					}
					
					remainingSteps = min( remainingSteps, nextSegment->maxSpeed );
				}
			}
			
//			Determine max speed according to markings
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
		
		void move() {
			vector< vector<StreetSegment> > &contents = streetMap->getContents();
			set<Vehicle*> processedVehicles;
			
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					Vehicle *v = contents[x][y].v;
					if( v != nullptr && processedVehicles.count( v ) == 0 ) {
						StreetSegment *previousSegment = nullptr;
						StreetSegment *nextSegment = &( contents[x][y] );
						for( long remainingSteps = v->currentSpeed; remainingSteps > 0;
						        --remainingSteps ) {
							previousSegment = nextSegment;
							if( previousSegment->isSink() ) {
//								Previous segment was sink and we are moving one further. Remove car.
								delete previousSegment->v;
								previousSegment->v = nullptr;
								break;
							}
							nextSegment = nextSegment->destinations[nextSegment->nextDestination];
							
							if( nextSegment->v != nullptr || nextSegment->mark != v ) {
								throw MessageException( "MOVE: invalid move, segment not empty \
								or not marked by moving car." );
							}
							nextSegment->v = v;
							previousSegment->v = nullptr;
							
							v->currentSpeed = min( v->currentSpeed, nextSegment->maxSpeed );
						}
						
						processedVehicles.insert( v );
					}
				}
			}
		}
};
