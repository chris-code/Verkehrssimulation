#pragma once

#include <vector>
#include <random>
#include "vehicle.hpp"

using namespace std;

class StreetmapSegment {
	public:
		StreetmapSegment() {
			maxSpeed = 1;
			currentDestinationID = 0;
			carGenerationRate = 0.15;
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
		
		StreetmapSegment * getCurrentDestination() {
			return destinations[currentDestinationID];
		}
		
		void addPredecessor( StreetmapSegment *pred ) {
			predecessors.push_back( pred );
		}
		
		void addDestination( StreetmapSegment *dest, double weight ) {
			destinations.push_back( dest );
			destinationWeights.push_back( weight );
		}
		
		void setDestinationWeight( StreetmapSegment *dest, double weight ) {
			for( long i = 0; i < long( destinations.size() ); ++i ) {
				if( destinations[i] == dest ) {
					destinationWeights[i] = weight;
					return;
				}
			}
			throw MessageException( "setDestinationWeight: destination not in destination list!" );
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
		double carGenerationRate;
		vector<StreetmapSegment*> predecessors;
		
		Vehicle *mark;
		Vehicle *v;
	private:
		vector<StreetmapSegment*> destinations;
		vector<double> destinationWeights;
		short currentDestinationID;
};
