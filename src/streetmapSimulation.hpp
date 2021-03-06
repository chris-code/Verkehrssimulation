#pragma once

#include <set>
#include <deque>
#include "messageException.hpp"
#include "streetmapRoad.hpp"
#include "streetmapVisualization.hpp"

using namespace std;

class StreetmapSimulation {
	public:
		StreetmapSimulation( default_random_engine &randomEngine,
		                     uniform_real_distribution<double> &dallyFactorDistribution,
		                     exponential_distribution<double> &riskFactorDistributionL2R,
		                     exponential_distribution<double> &riskFactorDistributionR2L,
		                     normal_distribution<double> &maxSpeedDistribution ) :
			randomEngine( randomEngine ),
			dallyFactorDistribution( dallyFactorDistribution ),
			riskFactorDistributionL2R( riskFactorDistributionL2R ),
			riskFactorDistributionR2L( riskFactorDistributionR2L ),
			maxSpeedDistribution( maxSpeedDistribution ) {
			streetMap = nullptr;
		}
		
		void simulate( StreetmapRoad &streetMap, double carGenerationRate, long iterations ) {
			this->streetMap = &streetMap;
			populateMap( carGenerationRate / 2.0 ); // Account for vehicles blocked from generation
			
			StreetmapVisualization vis( streetMap.getContents().size(),
			                            streetMap.getContents()[0].size() );
			vis.appendRoundabout( streetMap );
			
			for( long i = 0; i < iterations; ++i ) {
//				this->streetMap->visualize();
				simulateStep();
				vis.appendRoundabout( streetMap );
			}
			
			vis.save();
			
			this->streetMap = nullptr;
		}
		
	private:
		default_random_engine &randomEngine;
		uniform_real_distribution<double> dallyFactorDistribution;
		exponential_distribution<double> riskFactorDistributionL2R;
		exponential_distribution<double> riskFactorDistributionR2L;
		normal_distribution<double> maxSpeedDistribution;
		
		StreetmapRoad *streetMap;
		
		void simulateStep() {
			streetMap->drawDestinationsRandomly();
			streetMap->clearMarks();
			
			addCars();
			accelerate();
			checkDistances();
			dally();
			move();
		}
		
		void populateMap( double trafficDensity ) {
			vector< vector<StreetmapSegment> > &contents = streetMap->getContents();
			bernoulli_distribution carPlacementDistribution( trafficDensity );
			
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					if( ! contents[x][y].isDummy() ) {
						if( carPlacementDistribution( randomEngine ) ) {
							contents[x][y].v = new Vehicle( randomEngine, dallyFactorDistribution,
							                                riskFactorDistributionL2R,
							                                riskFactorDistributionR2L,
							                                maxSpeedDistribution );
						}
					}
				}
			}
		}
		
		void addCars() {
			set<StreetmapSegment*> &sources = streetMap->getSources();
			uniform_real_distribution<double> uniform01Distribution( 0.0, 1.0 );
			
			for( auto s = sources.begin(); s != sources.end(); ++s ) {
				if( ( *s )->v == nullptr &&
				        uniform01Distribution( randomEngine ) < ( *s )->carGenerationRate ) {
					( *s )->v = new Vehicle( randomEngine, dallyFactorDistribution,
					                         riskFactorDistributionL2R, riskFactorDistributionR2L,
					                         maxSpeedDistribution );
				}
			}
		}
		
		void accelerate() {
			vector< vector<StreetmapSegment> > &contents = streetMap->getContents();
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					if( contents[x][y].v != nullptr ) {
						contents[x][y].v->accelerate();
					}
				}
			}
		}
		
		StreetmapSegment * findSegmentWithVehicle( Vehicle *v ) {
			vector< vector<StreetmapSegment> > &contents = streetMap->getContents();
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					if( contents[x][y].v == v ) {
						return &( contents[x][y] );
					}
				}
			}
			throw MessageException( "No segment with this car!" );
		}
		
		void checkDistances() {
			vector< vector<StreetmapSegment> > &contents = streetMap->getContents();
			
//			Get segments that contain cars
			deque<StreetmapSegment*> segmentsWithCars = streetMap->getSegmentsWithCars();
			
//			Place markings along each Vehicle's path. Overwrite markigns according to right of way
			while( ! segmentsWithCars.empty() ) {
				StreetmapSegment *previousSegment = segmentsWithCars.front();
				Vehicle *v = previousSegment->v;
				segmentsWithCars.pop_front();
				
				long remainingSteps = min( previousSegment->v->currentSpeed,
				                           previousSegment->maxSpeed );
				while( remainingSteps != 0 ) {
					if( previousSegment->isSink() ) {
						break;
					}
					
					StreetmapSegment *nextSegment = previousSegment->getCurrentDestination();
					--remainingSteps;
					remainingSteps = min( remainingSteps, nextSegment->maxSpeed );
					if( nextSegment->v != nullptr ) {
						break;
					}
					
					if( nextSegment->mark != nullptr ) {
						if( nextSegment->predecessors[0] != previousSegment ) {
							remainingSteps = 0;
						} else {
							segmentsWithCars.push_back( findSegmentWithVehicle( nextSegment->mark ) );
							streetMap->clearMarks( nextSegment->mark );
							nextSegment->mark = v;
						}
					} else {
						nextSegment->mark = v;
					}
					
					previousSegment = nextSegment;
				}
			}
			
//			Determine max speed according to markings
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					if( contents[x][y].v != nullptr ) {
						segmentsWithCars.push_back( &( contents[x][y] ) );
					}
				}
			}
			
			while( ! segmentsWithCars.empty() ) {
				StreetmapSegment *currentSegment = segmentsWithCars.front();
				Vehicle *v = currentSegment->v;
				segmentsWithCars.pop_front();
				
				long remainingSteps = v->currentSpeed;
				long newSpeed = 0;
				while( remainingSteps != 0 ) {
					if( currentSegment->isSink() ) {
						++newSpeed;
						break;
					}
					
					currentSegment = currentSegment->getCurrentDestination();
					if( currentSegment->mark != v ) {
						remainingSteps = 0;
					} else {
						--remainingSteps;
						++newSpeed;
					}
				}
				
				v->currentSpeed = newSpeed;
			}
		}
		
		void dally() {
			vector< vector<StreetmapSegment> > &contents = streetMap->getContents();
			uniform_real_distribution<double> uniform01distribution( 0.0, 1.0 );
			
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					Vehicle *v = contents[x][y].v;
					if( v != nullptr ) {
						if( v->currentSpeed > 1 ) {
							if( uniform01distribution( randomEngine ) < v->dallyFactor ) {
								contents[x][y].v->accelerate( -1 );
							}
						}
					}
				}
			}
		}
		
		void move() {
			vector< vector<StreetmapSegment> > &contents = streetMap->getContents();
			set<Vehicle*> processedVehicles;
			
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					Vehicle *v = contents[x][y].v;
					if( v != nullptr && processedVehicles.count( v ) == 0 ) {
						StreetmapSegment *nextSegment = &( contents[x][y] );
						for( long remainingSteps = v->currentSpeed; remainingSteps > 0;
						        --remainingSteps ) {
							StreetmapSegment *previousSegment = nextSegment;
							if( previousSegment->isSink() ) {
//								Previous segment was sink and we are moving one further. Remove car.
								delete previousSegment->v;
								previousSegment->v = nullptr;
								break;
							}
							nextSegment = nextSegment->getCurrentDestination();
							
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
