#pragma once

#include <vector>
#include "Vehicle.hpp"

using namespace std;

class Road {
	public:
		Road(long streetLength, long laneCount) {
			resize(streetLength, laneCount);
		}

		long getStreetLength() {
			return streetLength;
		}

		long getLaneCount() {
			return laneCount;
		}

		void resize(long streetLength, long laneCount) {
			this->streetLength = streetLength;
			this->laneCount = laneCount;
			contents.resize(streetLength);
			for (long i = 0; i < long(contents.size()); ++i) {
				contents[i].resize(laneCount);
				for (long j = 0; j < long(contents[i].size()); ++j) {
					contents[i][j] = nullptr;
				}
			}
		}

		void insertVehicle(long s, long l, Vehicle* v) {
			contents[s][l] = v;
		}

		Vehicle* removeVehicle(long s, long l) {
			Vehicle* v = contents[s][l];
			contents[s][l] = nullptr;
			return v;
		}

	private:
		vector< vector<Vehicle*> > contents;
		long streetLength;
		long laneCount;
};
