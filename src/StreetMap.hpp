#pragma once

#include <vector>
#include <set>
#include <chrono>
#include <random>
#include <iostream> //FIXME remove this
#include "MessageException.hpp"
#include "StreetSegment.hpp"

using namespace std;

class StreetMap {
	public:
		StreetMap( long roundaboutWidth, long roundaboutHeight, long driveUpLength,
		           double trafficDensity, default_random_engine &randomEngine ) :
			randomEngine( randomEngine ) {
			
			buildMap( roundaboutWidth, roundaboutHeight, driveUpLength );
		}
		
		void drawDestinations() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					long choices = contents[x][y].destinations.size();
					if( choices > 1 ) {
						uniform_int_distribution<long> destinationDistribution( 0, choices - 1 );
						contents[x][y].nextDestination = destinationDistribution( randomEngine );
					}
				}
			}
		}
		
		void clearMarks() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					contents[x][y].mark = nullptr;
				}
			}
		}
		
		void buildMap( long roundaboutWidth, long roundaboutHeight, long driveUpLength ) {
			long buffer = 3;
			long driveUpSpeed = 4;
			dimX = ( driveUpLength + buffer ) * 2 + roundaboutWidth;
			dimY = ( driveUpLength + buffer ) * 1 + roundaboutHeight + buffer;
			
			contents.resize( dimX );
			for( long x = 0; x < dimX; ++x ) {
				contents[x].clear(); // Make sure we don't reuse segments (-> avoid broken pointers)
				contents[x].resize( dimY );
			}
			
			long leftBorder = driveUpLength + buffer + 1;
			long upperBorder = buffer + 1;
			long rightBorder = leftBorder + roundaboutWidth - 1;
			long lowerBorder = upperBorder + roundaboutHeight - 1;
			
			for( long x = leftBorder; x < rightBorder; ++x ) {
				contents[x][upperBorder].predecessors.push_back( & ( contents[x + 1][upperBorder] ) );
			}
			for( long x = rightBorder; x > leftBorder; --x ) {
				contents[x][lowerBorder].predecessors.push_back( &( contents[x - 1][lowerBorder] ) );
			}
			for( long y = upperBorder; y < lowerBorder; ++y ) {
				contents[rightBorder][y].predecessors.push_back( &( contents[rightBorder][y + 1] ) );
			}
			for( long y = lowerBorder; y > upperBorder; --y ) {
				contents[leftBorder][y].predecessors.push_back( &( contents[leftBorder][y - 1] ) );
			}
			
//			Build left drive up
			long leftDriveUpStart = leftBorder - driveUpLength;
			long leftDriveUpLower = upperBorder + roundaboutHeight / 2;
			long leftDriveUpUpper = leftDriveUpLower - 1;
			for( long x = leftDriveUpStart; x < leftBorder; ++x ) {
				contents[x][leftDriveUpUpper].predecessors.push_back(
				    & ( contents[x + 1][leftDriveUpUpper] ) );
				contents[x][leftDriveUpLower].predecessors.push_back(
				    & ( contents[x - 1][leftDriveUpLower] ) );
				    
				contents[x][leftDriveUpUpper].maxSpeed = driveUpSpeed;
				contents[x][leftDriveUpLower].maxSpeed = driveUpSpeed;
			}
			contents[leftBorder][leftDriveUpLower].predecessors.push_back(
			    &( contents[leftBorder - 1][leftDriveUpLower] ) );
			contents[leftDriveUpStart - 1][leftDriveUpLower].maxSpeed = driveUpSpeed;
			
//			Build right drive up
			long rightDriveUpStart = rightBorder + driveUpLength;
			long rightDriveUpLower = upperBorder + roundaboutHeight / 2;
			long rightDriveUpUpper = rightDriveUpLower - 1;
			for( long x = rightDriveUpStart; x > rightBorder; --x ) {
				contents[x][rightDriveUpUpper].predecessors.push_back(
				    &( contents[x + 1][rightDriveUpUpper] ) );
				contents[x][rightDriveUpLower].predecessors.push_back(
				    &( contents[x - 1][rightDriveUpLower] ) );
				    
				contents[x][rightDriveUpUpper].maxSpeed = driveUpSpeed;
				contents[x][rightDriveUpLower].maxSpeed = driveUpSpeed;
			}
			contents[rightBorder][rightDriveUpUpper].predecessors.push_back(
			    &( contents[rightBorder + 1][rightDriveUpUpper] ) );
			contents[rightDriveUpStart + 1][rightDriveUpUpper].maxSpeed = driveUpSpeed;
			
//			Build lower drive up
			long lowerDriveUpStart = lowerBorder + driveUpLength;
			long lowerDriveUpRight = leftBorder + roundaboutWidth / 2;
			long lowerDriveUpLeft = lowerDriveUpRight - 1;
			for( long y = lowerDriveUpStart; y > lowerBorder; --y ) {
				contents[lowerDriveUpLeft][y].predecessors.push_back(
				    &( contents[lowerDriveUpLeft][y - 1] ) );
				contents[lowerDriveUpRight][y].predecessors.push_back(
				    &( contents[lowerDriveUpRight][y + 1] ) );
				    
				contents[lowerDriveUpLeft][y].maxSpeed = driveUpSpeed;
				contents[lowerDriveUpRight][y].maxSpeed = driveUpSpeed;
			}
			contents[lowerDriveUpRight][lowerBorder].predecessors.push_back(
			    &( contents[lowerDriveUpRight][lowerBorder + 1] ) );
			contents[lowerDriveUpRight][lowerDriveUpStart+1].maxSpeed = driveUpSpeed;
			
			buildDestinationPointers();
			determineSourcesAndSinks();
		}
		
		double computeDensity() {
			long cells = 0;
			long vehicles = 0;
			
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					if( ! contents[x][y].isDummy() ) {
						++cells;
						if( contents[x][y].v != nullptr ) {
							++vehicles;
						}
					}
				}
			}
			
			return double( vehicles ) / double( cells );
		}
		
		void visualize() {
			for( long y = 0; y < dimY; ++y ) {
				for( long x = 0; x < dimX; ++x ) {
//					if( ! contents[x][y].isDummy() ) { //FIXME remove this
//						cout << contents[x][y].maxSpeed;
//						continue;
//					}
					if( contents[x][y].v != nullptr ) {
						cout << contents[x][y].v->currentSpeed;
					} else if( sources.count( &contents[x][y] ) ) {
						cout << "+";
					} else if( sinks.count( &contents[x][y] ) ) {
						cout << "-";
					} else if( ! contents[x][y].isDummy() ) {
						cout << "#";
					} else {
						cout << " ";
					}
				}
				cout << endl;
			}
		}
		
		set<StreetSegment*> &getSources() {
			return sources;
		}
		set<StreetSegment*> &getSinks() {
			return sinks;
		}
		
		vector< vector<StreetSegment> > &getContents() {
			return contents;
		}
		
	private:
		long dimX;
		long dimY;
		vector<vector<StreetSegment>> contents;
		set<StreetSegment*> sources;
		set<StreetSegment*> sinks;
		
		default_random_engine &randomEngine;
		
		void buildPredecessorPointers() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					long destinationCount = contents[x][y].destinations.size();
					for( long d = 0; d < destinationCount; ++d ) {
						contents[x][y].destinations[d]->predecessors.push_back( &( contents[x][y] ) );
					}
				}
			}
		}
		
		void buildDestinationPointers() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					long predecessorCount = contents[x][y].predecessors.size();
					for( long p = 0; p < predecessorCount; ++p ) {
						contents[x][y].predecessors[p]->destinations.push_back( &( contents[x][y] ) );
					}
				}
			}
		}
		
		void determineSourcesAndSinks() {
			sources.clear();
			sinks.clear();
			
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					if( contents[x][y].isSource() ) {
						sources.insert( &( contents[x][y] ) );
					}
					if( contents[x][y].isSink() ) {
						sinks.insert( &( contents[x][y] ) );
					}
				}
			}
		}
};
