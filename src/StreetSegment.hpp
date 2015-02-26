#pragma once

#include <vector>
#include "Vehicle.hpp"

using namespace std;

class StreetSegment {
	public:
		StreetSegment() {
			maxSpeed = 1;
			
			nextDestination = 0;
			mark = nullptr;
			v = nullptr;
		}
		
		bool isDummy() {
			if( destinations.size() == 0 && predecessors.size() == 0 ) {
				return true;
			}
			return false;
		}
		
		bool isSource() {
			if( ! isDummy() && predecessors.size() == 0 ) {
				return true;
			}
			return false;
		}
		
		bool isSink() {
			if( !isDummy() && destinations.size() == 0 ) {
				return true;
			}
			return false;
		}
		
		long maxSpeed;
		vector<StreetSegment*> destinations;
		vector<StreetSegment*> predecessors;
		
		short nextDestination;
		Vehicle *mark;
		Vehicle *v;
	private:
};
