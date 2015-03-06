#pragma once

#include <vector>
#include <set>
#include <deque>
#include <chrono>
#include <random>
#include <iostream> //TODO remove this
#include "MessageException.hpp"
#include "StreetSegment.hpp"
#include "Vehicle.hpp"

using namespace std;

class StreetMap {
	public:
		StreetMap( default_random_engine &randomEngine, long driveUpLength ) :
			randomEngine( randomEngine ) {
			dimX = 0;
			dimY = 0;
			buildTrumpetInterchange( driveUpLength );
		}
		StreetMap( long roundaboutWidth, long roundaboutHeight, long driveUpLength,
		           default_random_engine &randomEngine ) :
			randomEngine( randomEngine ) {
			dimX = 0;
			dimY = 0;
			buildRoundabout( roundaboutWidth, roundaboutHeight, driveUpLength );
		}
		virtual ~StreetMap() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					if( contents[x][y].v != nullptr ) {
						delete contents[x][y].v;
						contents[x][y].v = nullptr;
					}
				}
			}
		}
		
		void drawDestinationsRandomly() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					contents[x][y].drawDestinationRandomly( randomEngine );
				}
			}
		}
		
		void clearMarks( Vehicle *v = nullptr ) {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					if( v == nullptr || v == contents[x][y].mark )
						contents[x][y].mark = nullptr;
				}
			}
		}
		
		void buildRoundabout( long roundaboutWidth, long roundaboutHeight, long driveUpLength ) {
			long buffer = 3;
			long roundaboutSpeed = 1;
			long driveUpSpeed = 4;
			
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					if( contents[x][y].v != nullptr ) {
						delete contents[x][y].v;
						contents[x][y].v = nullptr;
					}
				}
			}
			
			dimX = ( driveUpLength + buffer ) * 2 + roundaboutWidth;
			dimY = ( driveUpLength + buffer ) * 1 + roundaboutHeight + buffer;
			contents.resize( dimX );
			for( long x = 0; x < dimX; ++x ) {
				contents[x].clear(); // Make sure we don't reuse segments
				contents[x].resize( dimY );
			}
			
//			Build roundabout
			long leftBorder = driveUpLength + buffer + 1;
			long upperBorder = buffer;
			long rightBorder = leftBorder + roundaboutWidth - 1;
			long lowerBorder = upperBorder + roundaboutHeight - 1;
			buildPredPointersInLine( leftBorder, upperBorder, rightBorder, upperBorder,
			                         roundaboutSpeed );
			buildPredPointersInLine( rightBorder, upperBorder, rightBorder, lowerBorder,
			                         roundaboutSpeed );
			buildPredPointersInLine( rightBorder, lowerBorder, leftBorder, lowerBorder,
			                         roundaboutSpeed );
			buildPredPointersInLine( leftBorder, lowerBorder, leftBorder, upperBorder,
			                         roundaboutSpeed );
			                         
//			Build left drive-up
			long leftDriveUpStart = leftBorder - driveUpLength;
			long leftDriveUpLower = upperBorder + roundaboutHeight / 2;
			long leftDriveUpUpper = leftDriveUpLower - 1;
			buildPredPointersInLine( leftDriveUpStart, leftDriveUpUpper, leftBorder,
			                         leftDriveUpUpper, driveUpSpeed );
			buildPredPointersInLine( leftBorder, leftDriveUpLower, leftDriveUpStart,
			                         leftDriveUpLower, driveUpSpeed );
			contents[leftBorder][leftDriveUpLower].maxSpeed = roundaboutSpeed;
			contents[leftBorder - 1][leftDriveUpLower].maxSpeed = roundaboutSpeed;
			contents[leftDriveUpStart][leftDriveUpLower].maxSpeed = driveUpSpeed;
			
//			Build right drive-up
			long rightDriveUpStart = rightBorder + driveUpLength;
			long rightDriveUpLower = upperBorder + roundaboutHeight / 2;
			long rightDriveUpUpper = rightDriveUpLower - 1;
			buildPredPointersInLine( rightBorder, rightDriveUpUpper, rightDriveUpStart,
			                         rightDriveUpUpper, driveUpSpeed );
			buildPredPointersInLine( rightDriveUpStart, rightDriveUpLower, rightBorder,
			                         rightDriveUpLower, driveUpSpeed );
			contents[rightBorder][rightDriveUpUpper].maxSpeed = roundaboutSpeed;
			contents[rightBorder + 1][rightDriveUpUpper].maxSpeed = roundaboutSpeed;
			contents[rightDriveUpStart][rightDriveUpUpper].maxSpeed = driveUpSpeed;
			
//			Build lower drive-up
			long lowerDriveUpStart = lowerBorder + driveUpLength;
			long lowerDriveUpRight = leftBorder + roundaboutWidth / 2;
			long lowerDriveUpLeft = lowerDriveUpRight - 1;
			buildPredPointersInLine( lowerDriveUpLeft, lowerDriveUpStart, lowerDriveUpLeft,
			                         lowerBorder, driveUpSpeed );
			buildPredPointersInLine( lowerDriveUpRight, lowerBorder, lowerDriveUpRight,
			                         lowerDriveUpStart, driveUpSpeed );
			contents[lowerDriveUpRight][lowerBorder].maxSpeed = roundaboutSpeed;
			contents[lowerDriveUpRight][lowerBorder + 1].maxSpeed = roundaboutSpeed;
			contents[lowerDriveUpRight][lowerDriveUpStart].maxSpeed = driveUpSpeed;
			
//			Generate other info from the current state
			buildDestinationPointers();
			determineSourcesAndSinks();
			
//			Set weights
			contents[leftBorder][leftDriveUpUpper].setDestinationWeight(
			    &( contents[leftBorder][leftDriveUpUpper + 1] ), 2.0 );
			contents[rightBorder][rightDriveUpLower].setDestinationWeight(
			    &( contents[rightBorder][rightDriveUpLower - 1] ), 2.0 );
			contents[lowerDriveUpLeft][lowerBorder].setDestinationWeight(
			    &( contents[lowerDriveUpLeft + 1][lowerBorder] ), 2.0 );
		}
		
//		Warning: You are about to enter the most repulsive piece of code you will ever read
		void buildTrumpetInterchange( long driveUpLength ) {
			long buffer = 3;
			long interchangeWidth = 52;
			long interchangeHeight = 37;
			long lowerLeftLoopSize = 18;
			long lowerRightLoopSize = lowerLeftLoopSize;
			
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					if( contents[x][y].v != nullptr ) {
						delete contents[x][y].v;
						contents[x][y].v = nullptr;
					}
				}
			}
			
			dimX = ( driveUpLength + buffer ) * 2 + interchangeWidth;
			dimY = buffer * 2 + driveUpLength + interchangeHeight;
			contents.resize( dimX );
			for( long x = 0; x < dimX; ++x ) {
				contents[x].clear(); // Make sure we don't reuse segments
				contents[x].resize( dimY );
			}
			
//			Set default speed
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					contents[x][y].maxSpeed = 4;
				}
			}
			
//			Build horizontal streets of the interchange
			long upperLoopLargeHeight = 15;
			long upperLoopLargeWidth = 22;
			long upperLaneY = upperLoopLargeHeight + buffer;
			long lowerLaneY = upperLaneY + 3;
			long horizontalLanesXstart = driveUpLength + buffer;
			for( long x = horizontalLanesXstart; x < horizontalLanesXstart + interchangeWidth;
			        ++x ) {
				contents[x][upperLaneY].addPredecessor( &( contents[x + 1][upperLaneY] ) );
				contents[x][lowerLaneY].addPredecessor( &( contents[x - 1][lowerLaneY] ) );
			}
			
//			Build left drive ups
			long driveUpLeftStart = buffer;
			for( long x = driveUpLeftStart; x < horizontalLanesXstart; ++x ) {
				contents[x][upperLaneY].addPredecessor( &( contents[x + 1][upperLaneY] ) );
				contents[x][lowerLaneY].addPredecessor( &( contents[x - 1][lowerLaneY] ) );
			}
			
//			Build right drive ups
			long driveUpRightEnd = 2 * driveUpLength + interchangeWidth;
			for( long x = horizontalLanesXstart + interchangeWidth; x < driveUpRightEnd; ++x ) {
				contents[x][upperLaneY].addPredecessor( &( contents[x + 1][upperLaneY] ) );
				contents[x][lowerLaneY].addPredecessor( &( contents[x - 1][lowerLaneY] ) );
			}
			
//			Build vertical streets of interchange
			long leftLaneX = interchangeWidth / 2 - 2 + ( driveUpLength + buffer );
			long rightLaneX = leftLaneX + 3;
			long driveUpLowerEnd = interchangeHeight + buffer + driveUpLength;
			for( long y = lowerLaneY + 3; y < driveUpLowerEnd; ++y ) {
				contents[leftLaneX][y].addPredecessor( &( contents[leftLaneX][y - 1] ) );
				contents[rightLaneX][y].addPredecessor( &( contents[rightLaneX][y + 1] ) );
				if( y < lowerLaneY + lowerLeftLoopSize + 6 ) {
					contents[leftLaneX][y].maxSpeed = 3;
					contents[rightLaneX][y].maxSpeed = 3;
				}
			}
			long hOffset = 10;
			long vOffset = - 10;
			for( long y = lowerLaneY - 5; y < lowerLaneY + 3; ++y ) {
				contents[leftLaneX + hOffset][y + vOffset].addPredecessor(
				    &( contents[leftLaneX + hOffset][y - 1 + vOffset] ) );
				contents[rightLaneX + hOffset][y + vOffset].addPredecessor(
				    &( contents[rightLaneX + hOffset][y + 1 + vOffset] ) );
				contents[leftLaneX + hOffset][y + vOffset].maxSpeed = 3;
				contents[rightLaneX + hOffset][y + vOffset].maxSpeed = 3;
			}
			contents[leftLaneX + hOffset][lowerLaneY - 5 - 1 + vOffset].addPredecessor(
			    &( contents[leftLaneX][lowerLaneY - 6] ) );
			contents[leftLaneX + hOffset][lowerLaneY - 5 - 1 + vOffset].maxSpeed = 3;
			contents[leftLaneX][lowerLaneY + 2].addPredecessor(
			    &( contents[leftLaneX + hOffset][lowerLaneY + 2 + vOffset] ) );
			contents[leftLaneX][lowerLaneY + 2].maxSpeed = 3;
			contents[rightLaneX][lowerLaneY - 5].addPredecessor(
			    &( contents[rightLaneX + hOffset][lowerLaneY - 5 + vOffset] ) );
			contents[rightLaneX][lowerLaneY - 5].maxSpeed = 3;
			contents[rightLaneX + hOffset][lowerLaneY + 3 + vOffset].addPredecessor(
			    &( contents[rightLaneX][lowerLaneY + 3] ) );
			contents[rightLaneX + hOffset][lowerLaneY + 3 + vOffset].maxSpeed = 3;
			for( long y = buffer; y < lowerLaneY - 5; ++y ) {
				if( y > buffer + 5 ) {
					contents[leftLaneX][y].addPredecessor( &( contents[leftLaneX][y - 1] ) );
					if( y > buffer + 8 ) {
						contents[leftLaneX][y].maxSpeed = 3;
					} else {
						contents[leftLaneX][y].maxSpeed = 2;
					}
				}
				contents[rightLaneX][y].addPredecessor( &( contents[rightLaneX][y + 1] ) );
				contents[rightLaneX][y].maxSpeed = 3;
			}
			
//			Build horizontal segments of upper outer loop
			long upperOuterLoopStartX = rightLaneX - upperLoopLargeWidth + 1;
			for( long x = upperOuterLoopStartX; x < rightLaneX; ++x ) {
				contents[x][buffer].addPredecessor( &( contents[x + 1][buffer] ) );
				contents[x][buffer].maxSpeed = 3;
			}
//			Build vertical segments of upper outer loop
			for( long y = buffer + 1; y <= upperLaneY; ++y ) {
				contents[upperOuterLoopStartX][y].addPredecessor(
				    &( contents[upperOuterLoopStartX][y - 1] ) );
				if( y < upperLaneY ) {
					contents[upperOuterLoopStartX][y].maxSpeed = 3;
				}
			}
			
//			Build horizontal segments of upper inner loop
			long upperLoopSmallWidth = 10;
			long upperInnerLoopStartX = leftLaneX - upperLoopSmallWidth + 1;
			for( long x = upperInnerLoopStartX; x <= leftLaneX; ++x ) {
				contents[x][buffer + 5].addPredecessor( &( contents[x - 1][buffer + 5] ) );
				contents[x][buffer + 5].maxSpeed = 2;
			}
//			Build vertical segments of upper inner loop
			for( long y = buffer + 5; y < upperLaneY; ++y ) {
				contents[upperInnerLoopStartX - 1][y].addPredecessor(
				    &( contents[upperInnerLoopStartX - 1][y + 1] ) );
				if( y < buffer + 9 ) {
					contents[upperInnerLoopStartX - 1][y].maxSpeed = 2;
				} else {
					contents[upperInnerLoopStartX - 1][y].maxSpeed = 3;
				}
			}
			
//			Build lower left loop
			long lowerLeftLoopHorizY = lowerLaneY + lowerLeftLoopSize;
			long lowerLeftLoopVertX = leftLaneX - lowerLeftLoopSize;
			for( long x = lowerLeftLoopVertX + 1; x <= leftLaneX; ++x ) {
				contents[x][lowerLeftLoopHorizY].addPredecessor(
				    &( contents[x - 1][lowerLeftLoopHorizY] ) );
				if( x < leftLaneX ) {
					contents[x][lowerLeftLoopHorizY].maxSpeed = 3;
				}
			}
			for( long y = lowerLaneY + 1; y <= lowerLeftLoopHorizY; ++y ) {
				contents[lowerLeftLoopVertX][y].addPredecessor(
				    &( contents[lowerLeftLoopVertX][y - 1] ) );
				contents[lowerLeftLoopVertX][y].maxSpeed = 3;
			}
			
//			Build lower right loop
			long lowerRightLoopHorizY = lowerLaneY + lowerRightLoopSize;
			long lowerRightLoopVertX = rightLaneX + lowerRightLoopSize;
			for( long x = rightLaneX + 1; x <= lowerRightLoopVertX; ++x ) {
				contents[x][lowerRightLoopHorizY].addPredecessor(
				    &( contents[x - 1][lowerRightLoopHorizY] ) );
				contents[x][lowerRightLoopHorizY].maxSpeed = 3;
			}
			for( long y = lowerLaneY; y < lowerRightLoopHorizY; ++y ) {
				contents[lowerRightLoopVertX][y].addPredecessor(
				    &( contents[lowerRightLoopVertX][y + 1] ) );
				if( y != lowerLaneY ) {
					contents[lowerRightLoopVertX][y].maxSpeed = 3;
				}
			}
			
//			Generate other info from the current state
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
//					if( ! contents[x][y].isDummy() ) {
//						cout << contents[x][y].maxSpeed;
//						continue;
//					}
					if( contents[x][y].v != nullptr ) {
						cout << contents[x][y].v->currentSpeed;
					} else if( contents[x][y].isSource() ) {
						cout << "+";
					} else if( contents[x][y].isSink() ) {
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
		
		deque<StreetSegment*> getSegmentsWithCars() {
			deque<StreetSegment*> segmentsWithCars;
			for( long x = 0; x < long( contents.size() ); ++x ) {
				for( long y = 0; y < long( contents[x].size() ); ++y ) {
					if( contents[x][y].v != nullptr ) {
						segmentsWithCars.push_back( &( contents[x][y] ) );
					}
				}
			}
			return segmentsWithCars;
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
		default_random_engine &randomEngine;
		long dimX;
		long dimY;
		vector<vector<StreetSegment>> contents;
		set<StreetSegment*> sources;
		set<StreetSegment*> sinks;
		
//		Builds a line of predecessor pointers that point in the direction from (startX,startY) to
//		(stopX,stopY).
//		(startX,startY) is the first cell to receive such a pointer, and (stopX,stopY) is the first
//		cell not to receive one.
//		maxSpeed is set for exactly those cells that get pred pointers
		void buildPredPointersInLine( long startX, long startY, long stopX, long stopY,
		                              long maxSpeed ) {
			if( startX == stopX ) { // vertical
				long x = startX;
				if( startY < stopY ) { // downwards
					for( long y = startY; y < stopY; ++y ) {
						contents[x][y].addPredecessor( &( contents[x][y + 1] ) );
						contents[x][y].maxSpeed = maxSpeed;
					}
				} else { // upwarsd
					for( long y = startY; y > stopY; --y ) {
						contents[x][y].addPredecessor( &( contents[x][y - 1] ) );
						contents[x][y].maxSpeed = maxSpeed;
					}
				}
			} else if( startY == stopY ) { // horizontal
				long y = startY;
				if( startX < stopX ) { // to the right
					for( long x = startX; x < stopX; ++x ) {
						contents[x][y].addPredecessor( &( contents[x + 1][y] ) );
						contents[x][y].maxSpeed = maxSpeed;
					}
				} else { // to the left
					for( long x = startX; x > stopX; --x ) {
						contents[x][y].addPredecessor( &( contents[x - 1][y] ) );
						contents[x][y].maxSpeed = maxSpeed;
					}
				}
			} else { // no direction can be infered from the start-stop coordinates
				throw MessageException( "buildPredPointersInLine: start and stop not aligned" );
			}
		}
		
		void buildDestinationPointers() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					long predecessorCount = contents[x][y].predecessors.size();
					for( long p = 0; p < predecessorCount; ++p ) {
						contents[x][y].predecessors[p]->addDestination( &( contents[x][y] ), 1 );
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
