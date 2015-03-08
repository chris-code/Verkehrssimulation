#pragma once

#include <vector>
#include <set>
#include <deque>
#include <chrono>
#include <random>
#include <iostream> //TODO remove this
#include "messageException.hpp"
#include "streetmapSegment.hpp"
#include "vehicle.hpp"

using namespace std;

class StreetMap {
	public:
		StreetMap( default_random_engine &randomEngine, long driveUpLength,
		           double carGenerationRate ) :
			randomEngine( randomEngine ) {
			dimX = 0;
			dimY = 0;
			buildTrumpetInterchange( driveUpLength, carGenerationRate );
		}
		StreetMap( long roundaboutWidth, long roundaboutHeight, long driveUpLength,
		           double carGenerationRate, default_random_engine &randomEngine ) :
			randomEngine( randomEngine ) {
			dimX = 0;
			dimY = 0;
			buildRoundabout( roundaboutWidth, roundaboutHeight, driveUpLength, carGenerationRate );
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
		
		void buildRoundabout( long roundaboutWidth, long roundaboutHeight, long driveUpLength,
		                      double carGenerationRate ) {
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
			determineSourcesAndSinks( carGenerationRate );
			
//			Set weights
			contents[leftBorder][leftDriveUpUpper].setDestinationWeight(
			    &( contents[leftBorder][leftDriveUpUpper + 1] ), 2.0 );
			contents[rightBorder][rightDriveUpLower].setDestinationWeight(
			    &( contents[rightBorder][rightDriveUpLower - 1] ), 2.0 );
			contents[lowerDriveUpLeft][lowerBorder].setDestinationWeight(
			    &( contents[lowerDriveUpLeft + 1][lowerBorder] ), 2.0 );
		}
		
		/*
		 *      A-----------B
		 *      |           |
		 *      |    C---D  |
		 *      |    |   |  |
		 *      |    |   |  |
		 *      |    |   E  F
		 *      |    |
		 * G----H----I---J--K-------------L
		 * M----N--------------------O----P
		 *      |                    |
		 *      |        Q  R        |
		 *      |        |  |        |
		 *      |        |  |        |
		 *      S--------T  U--------V
		 *               |  |
		 *               |  |
		 *               X  Y
		 */
		void buildTrumpetInterchange( long driveUpLength, double carGenerationRate ) {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					if( contents[x][y].v != nullptr ) {
						delete contents[x][y].v;
						contents[x][y].v = nullptr;
					}
				}
			}
			
			long tightLoopSpeed = 2;
			long wideLoopSpeed = 3;
			long interchangeSpeed = 4;
			
			long buffer = 3;
			long lowerLoopSize = 18;
			long upperInnerLoopSize = 10;
			long upperOuterLoopHeight = 15;
			
			long interchangeWidth = 2 * lowerLoopSize + 2;
			long interchangeHeight = lowerLoopSize + upperOuterLoopHeight + 2;
			dimX = interchangeWidth + 2 * driveUpLength + 2 * buffer;
			dimY = interchangeHeight + driveUpLength + 2 * buffer;
			contents.resize( dimX );
			for( long x = 0; x < dimX; ++x ) {
				contents[x].clear(); // Make sure we don't reuse segments
				contents[x].resize( dimY );
			}
//			Set default speed
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					contents[x][y].maxSpeed = interchangeSpeed;
				}
			}
			
			long dejqtxX = buffer + driveUpLength + lowerLoopSize;
			long ciX = dejqtxX - upperInnerLoopSize;
			long ahnsX = dejqtxX - lowerLoopSize;
			long gmX = ahnsX - driveUpLength;
			long bfkruyX = dejqtxX + 3;
			long ovX = bfkruyX + lowerLoopSize;
			long lpX = ovX + driveUpLength;
			
			long ghijklY = buffer + upperOuterLoopHeight;
			long abY = ghijklY - upperOuterLoopHeight;
			long cdY = ghijklY - upperInnerLoopSize;
			long efY = ghijklY - 2;
			long mnopY = ghijklY + 3;
			long qrY = mnopY + 2;
			long stuvY = mnopY + lowerLoopSize;
			long xyY = stuvY + driveUpLength;
			
			pair<long, long> a( ahnsX, abY );
			pair<long, long> b( bfkruyX, abY );
			pair<long, long> c( ciX, cdY );
			pair<long, long> d( dejqtxX, cdY );
			pair<long, long> e( dejqtxX, efY );
			pair<long, long> f( bfkruyX, efY );
			pair<long, long> g( gmX, ghijklY );
			pair<long, long> h( ahnsX, ghijklY );
			pair<long, long> i( ciX, ghijklY );
			pair<long, long> j( dejqtxX, ghijklY );
			pair<long, long> k( bfkruyX, ghijklY );
			pair<long, long> l( lpX, ghijklY );
			pair<long, long> m( gmX, mnopY );
			pair<long, long> n( ahnsX, mnopY );
			pair<long, long> o( ovX, mnopY );
			pair<long, long> p( lpX, mnopY );
			pair<long, long> q( dejqtxX, qrY );
			pair<long, long> r( bfkruyX, qrY );
			pair<long, long> s( ahnsX, stuvY );
			pair<long, long> t( dejqtxX, stuvY );
			pair<long, long> u( bfkruyX, stuvY );
			pair<long, long> v( ovX, stuvY );
			pair<long, long> x( dejqtxX, xyY );
			pair<long, long> y( bfkruyX, xyY );
			
//			Horizontal main streets
			buildPredPointersInLine( g, l, interchangeSpeed );
			buildPredPointersInLine( p, m, interchangeSpeed );
			
//			Vertical main streets
			buildPredPointersInLine( r, y, wideLoopSpeed );
			buildPredPointersInLine( x, q, wideLoopSpeed );
			buildPredPointersInLine( b, f, wideLoopSpeed );
			buildPredPointersInLine( e, d, tightLoopSpeed );
			for( long yPos = stuvY + 5; yPos <= xyY; ++yPos ) {
				contents[dejqtxX][yPos].maxSpeed = interchangeSpeed;
				contents[bfkruyX][yPos].maxSpeed = interchangeSpeed;
			}
			
//			Horizontal parts of upper loops
			buildPredPointersInLine( a, b, wideLoopSpeed );
			buildPredPointersInLine( d, c, tightLoopSpeed );
			
//			Vertical parts of upper loops
			buildPredPointersInLine( h , a, wideLoopSpeed );
			buildPredPointersInLine( c, i, tightLoopSpeed );
			contents[h.first][h.second].maxSpeed = interchangeSpeed;
			for( long yPos = ghijklY - 5; yPos < ghijklY; ++yPos ) {
				contents[ciX][yPos].maxSpeed = wideLoopSpeed;
				contents[dejqtxX][yPos].maxSpeed = wideLoopSpeed;
			}
			
//			Lower right loop
			buildPredPointersInLine( o, v, wideLoopSpeed );
			buildPredPointersInLine( v, u, wideLoopSpeed );
			contents[o.first][o.second].maxSpeed = interchangeSpeed;
			
//			Lower left loop
			buildPredPointersInLine( t, s, wideLoopSpeed );
			buildPredPointersInLine( s, n, wideLoopSpeed );
			
//			Build tunnel next to the intersection
			pair<long, long> tunnelLeftLower( bfkruyX + 5, ghijklY - 4 );
			pair<long, long> tunnelLeftUpper( bfkruyX + 5, tunnelLeftLower.second - 5 );
			pair<long, long> tunnelRightLower( tunnelLeftLower.first + 3, tunnelLeftLower.second );
			pair<long, long> tunnelRightUpper( tunnelRightLower.first, tunnelRightLower.second - 5 );
			buildPredPointersInLine( tunnelLeftLower, tunnelLeftUpper, wideLoopSpeed );
			buildPredPointersInLine( tunnelRightUpper, tunnelRightLower, wideLoopSpeed );
			
			contents[tunnelLeftUpper.first][tunnelLeftUpper.second].addPredecessor( &
			        ( contents[e.first][e.second] ) );
			contents[q.first][q.second].addPredecessor( &
			        ( contents[tunnelLeftLower.first][tunnelLeftLower.second] ) );
			contents[f.first][f.second].addPredecessor( &
			        ( contents[tunnelRightUpper.first][tunnelRightUpper.second] ) );
			contents[tunnelRightLower.first][tunnelRightLower.second].addPredecessor( &
			        ( contents[r.first][r.second] ) );
			contents[q.first][q.second].maxSpeed = wideLoopSpeed;
			contents[tunnelLeftUpper.first][tunnelLeftUpper.second].maxSpeed = wideLoopSpeed;
			contents[tunnelRightLower.first][tunnelRightLower.second].maxSpeed = wideLoopSpeed;
			contents[f.first][f.second].maxSpeed = wideLoopSpeed;
			
//			Generate other info from the current state
			buildDestinationPointers();
			determineSourcesAndSinks( carGenerationRate );
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
					if( ! contents[x][y].isDummy() ) {
						cout << contents[x][y].maxSpeed;
						continue;
					}
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
		
		void buildPredPointersInLine( pair<long, long> &start, pair<long, long> &stop,
		                              long maxSpeed ) {
			buildPredPointersInLine( start.first, start.second, stop.first, stop.second, maxSpeed );
		}
		
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
		
		void determineSourcesAndSinks( double carGenerationRate ) {
			sources.clear();
			sinks.clear();
			
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					if( contents[x][y].isSource() ) {
						sources.insert( &( contents[x][y] ) );
						contents[x][y].carGenerationRate = carGenerationRate;
					}
					if( contents[x][y].isSink() ) {
						sinks.insert( &( contents[x][y] ) );
					}
				}
			}
		}
};
