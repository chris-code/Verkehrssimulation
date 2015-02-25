#pragma once

#include<vector>
#include<chrono>
#include<random>
#include<iostream> //FIXME remove this
#include "MessageException.hpp"
#include "StreetSegment.hpp"

using namespace std;

class StreetMap {
	public:
		StreetMap( long dimX, long dimY, default_random_engine &randomEngine ) :
			randomEngine( randomEngine ) {
			this->dimX = dimX;
			this->dimY = dimY;
			contents.resize( dimX );
			for( long x = 0; x < dimX; ++x ) {
				contents[x].resize( dimY );
			}
			
			initialize();
			drawDestinations();
			visualize();
		}
		
		void drawDestinations() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					long choices = contents[x][y].destinations.size();
					uniform_int_distribution<long> destinationDistribution( 0, choices - 1 );
					contents[x][y].nextDestination = destinationDistribution( randomEngine );
				}
			}
		}
		
		void clearMarks() {
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					contents[x][y].mark = false;
				}
			}
		}
		
		void initialize() { //TODO set max speed
			if( dimX != 70 || dimY != 70 ) {
				throw MessageException( "Can't initialize: Unrecognized dimensions!" );
			}
			
			// Roundabout is spanned by [30,30] and [37,35]
			for( long x = 31; x <= 37; ++x ) {
				contents[x][30].destinations.push_back( &( contents[x - 1][30] ) );
			}
			for( long x = 30; x <= 36; ++x ) {
				contents[x][35].destinations.push_back( &( contents[x + 1][35] ) );
			}
			for( long y = 30; y <= 34; ++y ) {
				contents[30][y].destinations.push_back( &( contents[30][y + 1] ) );
			}
			for( long y = 31; y <= 35; ++y ) {
				contents[37][y].destinations.push_back( &( contents[37][y - 1] ) );
			}
			
			// Create roads
			for( long x = 5; x < 30; ++x ) {
				contents[x][32].destinations.push_back( &( contents[x - 1][32] ) );
				contents[x][33].destinations.push_back( &( contents[x + 1][33] ) );
			}
			contents[30][32].destinations.push_back( &( contents[30 - 1][32] ) );
			
			for( long x = 38; x < 63; ++x ) {
				contents[x][32].destinations.push_back( &( contents[x - 1][32] ) );
				contents[x][33].destinations.push_back( &( contents[x + 1][33] ) );
			}
			contents[37][33].destinations.push_back( &( contents[37 + 1][33] ) );
			
			for( long y = 36; y < 61; ++y ) {
				contents[33][y].destinations.push_back( &( contents[33][y + 1] ) );
				contents[34][y].destinations.push_back( &( contents[34][y - 1] ) );
			}
			contents[33][35].destinations.push_back( &( contents[33][35 + 1] ) );
			
			buildSourcePointers();
		}
		
		void visualize() {
			for( long y = 0; y < dimY; ++y ) {
				for( long x = 0; x < dimX; ++x ) {
					if(contents[x][y].v != nullptr) {
						cout << contents[x][y].v->currentSpeed;
					} else if(contents[x][y].destinations.size() > 0) {
						cout << "#";
					}
					else {
						cout << " ";
					}
				}
				cout << endl;
			}
		}
	private:
		long dimX;
		long dimY;
		vector<vector<StreetSegment>> contents;
		
		default_random_engine &randomEngine;
		
		void buildSourcePointers() {
			//TODO build sources
			for( long x = 0; x < dimX; ++x ) {
				for( long y = 0; y < dimY; ++y ) {
					long destinationCount = contents[x][y].destinations.size();
					for( long d = 0; d < destinationCount; ++d ) {
						contents[x][y].destinations[d]->sources.push_back( &( contents[x][y] ) );
					}
				}
			}
		}
};
