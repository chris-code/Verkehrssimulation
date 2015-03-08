#pragma once

#include <vector>
#include "messageException.hpp"
#include "vehicle.hpp"

using namespace std;

class Road {
	public:
		Road(long streetLength, long laneCount) {
			resize(streetLength, laneCount);
		}
		virtual ~Road() {
			clear();
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
			if(contents[fromS][fromL] == nullptr) {
				throw MessageException("Invalid move: source is empty");
			}
			if(toS >= streetLength) { // Car leaves the road
				Vehicle *v = removeVehicle(fromS, fromL);
				delete v;
				return;
			}
			
			if(toS < 0 || toS >= streetLength || toL < 0 || toL >= laneCount) {
				throw MessageException("Invalid move: destination not in road");
			}
			if(contents[toS][toL] != nullptr) {
				if(fromS == toS && fromL == toL) {
					throw MessageException("Invalid move: self-replacement!");
				}
				throw MessageException("Invalid move: target not empty");
			}
			Vehicle *v = contents[fromS][fromL];
			contents[fromS][fromL] = nullptr;
			contents[toS][toL] = v;
		}

		Vehicle* removeVehicle(long s, long l) {
			if(s < 0 || s >= streetLength || l < 0 || l >= laneCount) {
				throw MessageException("Invalid remove: cell not in road");
			}
			if(contents[s][l] == nullptr) {
				throw MessageException("Invalid remove: cell is empty");
			}
			Vehicle* v = contents[s][l];
			contents[s][l] = nullptr;
			return v;
		}

		Vehicle* getVehicle(long s, long l) {
			return contents[s][l];
		}
		
		double computeDensity() {
			double density = 0.0;
			for (long s = 0; s < long(contents.size()); ++s) {
				for (long l = 0; l < long(contents[s].size()); ++l) {
					if(contents[s][l] != nullptr) {
						density += 1.0;
					}
				}
			}
			return density / (streetLength * laneCount);
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
