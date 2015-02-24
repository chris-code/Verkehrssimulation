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
			if(s < 0 || s >= streetLength || l < 0 || l >= laneCount) {
				throw MessageException("Invalid insert: cell not in road");
			}
			if(contents[s][l] != nullptr) {
				throw MessageException("Invalid insert: cell is not empty");
			}
			contents[s][l] = v;
		}

		void moveVehicle(long fromS, long fromL, long toS, long toL) {			
			if(fromS < 0 || fromS >= streetLength || fromL < 0 || fromL >= laneCount) {
				throw MessageException("Invalid move: source not in road");
			}
			if(toS < 0 || toS >= streetLength || toL < 0 || toL >= laneCount) {
				throw MessageException("Invalid move: destination not in road");
			}
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
				throw MessageException("Invalid remove: cell is empty");
			}
			if(s < 0 || s >= streetLength || l < 0 || l >= laneCount) {
				throw MessageException("Invalid remove: cell not in road");
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
