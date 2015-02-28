#pragma once

#include <vector>
#include <random>
#include "Vehicle.hpp"

using namespace std;

class StreetSegment {
	public:
		StreetSegment() {
			maxSpeed = 1;
			currentDestinationID = 0;
			mark = nullptr;
			v = nullptr;
		}
		
		void drawDestinationRandomly( default_random_engine &randomEngine ) {
			if( destinations.size() > 1 ) {
				discrete_distribution<long> destinationDistribution( destinationWeights.begin(),
				        destinationWeights.end() );
				currentDestinationID = destinationDistribution( randomEngine );
			}
		}
		
		StreetSegment * getCurrentDestination() {
			return destinations[currentDestinationID];
		}
		
		void addPredecessor( StreetSegment *pred ) {
			predecessors.push_back( pred );
		}
		
		void addDestination( StreetSegment *dest, double weight ) {
			destinations.push_back( dest );
			destinationWeights.push_back( weight );
		}
		
		void setDestinationWeight( StreetSegment *dest, double weight ) {
			for( long i = 0; i < long( destinations.size() ); ++i ) {
				if( destinations[i] == dest ) {
					destinationWeights[i] = weight;
				}
			}
		}
		
		bool isDummy() {
			return destinations.size() == 0 && predecessors.size() == 0;
		}
		
		bool isSource() {
			return !isDummy() && predecessors.size() == 0;
		}
		
		bool isSink() {
			return !isDummy() && destinations.size() == 0;
		}
		
		long maxSpeed;
		vector<StreetSegment*> predecessors;
		
		Vehicle *mark;
		Vehicle *v;
	private:
		vector<StreetSegment*> destinations;
		vector<double> destinationWeights;
		short currentDestinationID;
};
