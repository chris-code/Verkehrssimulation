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
			clear();

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

		Vehicle* getVehicle(long s, long l) {
			return contents[s][l];
		}

	private:
		void clear() {
			for (long s = 0; s < long(contents.size()); ++s) {
				for (long l = 0; l < long(contents[s].size()); ++l) {
					if (contents[s][l] != nullptr) {
						delete contents[s][l];
						contents[s][l] = nullptr;
					}
				}
			}
		}

		vector< vector<Vehicle*> > contents;
		long streetLength;
		long laneCount;
};
