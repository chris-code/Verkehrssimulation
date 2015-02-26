#pragma once

#include <vector>
#include "Vehicle.hpp"

using namespace std;

class StreetSegment {
	public:
		StreetSegment() {
			maxSpeed = 1;
			
			nextDestination = 0;
			mark = false;
			v = nullptr;
		}
		
		bool isDummy() {
			if( destinations.size() == 0 ) {
				return true;
			}
			return false;
		}
		
		long maxSpeed;
		vector<StreetSegment*> destinations;
		vector<StreetSegment*> sources;
		
		short nextDestination;
		bool mark;
		Vehicle *v;
	private:
};
