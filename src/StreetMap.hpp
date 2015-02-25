#pragma once

#include<vector>
#include<chrono>
#include<random>
#include "StreetSegment.hpp"

using namespace std;

class StreetMap {
	public:
		StreetMap( long dimX, long dimY ) :
			randomEngine( chrono::system_clock::now().time_since_epoch().count() ) {
			this->dimX = dimX;
			this->dimY = dimY;
			contents.resize( dimX );
			for( long x = 0; x < dimX; ++x ) {
				contents[x].resize( dimY );
			}
		}
		
		void setDestinations() {
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
	private:
		long dimX;
		long dimY;
		vector<vector<StreetSegment>> contents;
		
		default_random_engine randomEngine;
};
