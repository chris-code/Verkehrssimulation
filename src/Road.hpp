#pragma once

#include <vector>
#include "MessageException.hpp"
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
			if(contents[s][l] != nullptr) {
				throw MessageException("Invalid insertion");
			}
			contents[s][l] = v;
		}

		void moveVehicle(long fromS, long fromL, long toS, long toL) {			
			if(contents[fromS][fromL] == nullptr) {
				throw MessageException("Invalid move: source is empty");
			}
			if(contents[toS][toL] != nullptr) {
				throw MessageException("Invalid move: target not empty");
			}
			
			Vehicle *v = contents[fromS][fromL];
			contents[fromS][fromL] = nullptr;
			contents[toS][toL] = v;
		}

		Vehicle* removeVehicle(long s, long l) {
			if(contents[s][l] == nullptr) {
				throw MessageException("Invalid remove");
			}
			Vehicle* v = contents[s][l];
			contents[s][l] = nullptr;
			return v;
		}

	private:
		vector< vector<Vehicle*> > contents;
		long streetLength;
		long laneCount;
};
