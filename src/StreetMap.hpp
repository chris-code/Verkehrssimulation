#include <vector>
#include <random>
#include <chrono>
#include "StreetSegment.hpp"

using namespace std;

class StreetMap {
	public:
		StreetMap(long xSize, long ySize) {
			randomEngine = default_random_engine(chrono::system_clock::now().time_since_epoch().count());

			data.resize(xSize);

			for (auto x = 0L; x < xSize; ++x) {
				for (auto y = 0L; y < ySize; ++y) {
					StreetSegment ss(randomEngine);
					data[x].push_back(ss);
				}
			}

		}
		StreetSegment & operator()(long x, long y) {
			return data[x][y];
		}
	private:
		vector< vector< StreetSegment > > data;
		default_random_engine randomEngine;
};
