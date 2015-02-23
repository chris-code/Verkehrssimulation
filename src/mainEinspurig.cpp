#include <cstdlib>
#include <iostream>
#include <random>
#include <chrono>
#include "StreetMap.hpp"
#include "Grid.hpp"
#include "CImg.h"

using namespace std;
namespace cn = cimg_library;

cn::CImg<unsigned char> gridToImg(Grid<char>& grid) {
	cn::CImg<unsigned char> img(grid.getXsize(), grid.getYsize());

	for(auto x = 0L; x < grid.getXsize(); ++x) {
		for(auto y = 0L; y < grid.getYsize(); ++y) {
			img(x, y) = grid(x, y) + 1;
		}
	}

	return img;
}


cn::CImg<unsigned char> append(cn::CImg<unsigned char>& bigImg, cn::CImg<unsigned char>& smallImg) {
	return bigImg.get_append(smallImg, 'y', 0);
}


Grid<char> generateStreet(default_random_engine &randomEngine, uniform_real_distribution<double> &uniform01,
                          uniform_int_distribution<long> &uniform05, long segmentCount, double carDensity) {
	Grid<char> street(segmentCount, 1, -1);
	for(auto seg = 0L; seg < street.getXsize(); ++seg) {
		if(uniform01(randomEngine) < carDensity) {
			street(seg, 0) = uniform05(randomEngine); //TODO randomize speed
		}
	}

	return street;
}

void simulateStreetStep(default_random_engine &randomEngine, uniform_real_distribution<double> &uniform01,
                        uniform_int_distribution<long> &uniform05, Grid<char> &street, double carGenerationRate,
                        double p) {
//	Accellerate
	for(auto seg = 0L; seg < street.getXsize(); ++seg) {
		if(street(seg, 0) != -1 && street(seg, 0) < 5) {
			street(seg, 0) = street(seg, 0) + 1;
		}
	}

//	Slowing down
	for(auto seg = 0L; seg < street.getXsize(); ++seg) {
		if(street(seg, 0) != -1) {
			char speed = street(seg, 0);
			for(auto offset = 1; offset <= speed; ++offset) {
				if(seg + offset >= street.getXsize()) {
					break; // Don't slow down if leaving street
				}
				if(street(seg + offset, 0) != -1) {
					street(seg, 0) = offset - 1;
					break;
				}
			}
		}
	}

//	Randomization
	for(auto seg = 0L; seg < street.getXsize(); ++seg) {
		if(street(seg, 0) > 0) {
			if(uniform01(randomEngine) < p) {
				street(seg, 0) = street(seg, 0) - 1;
			}
		}
	}

//	Car motion
//	for(auto seg = 0L; seg < street.getXsize(); ++seg) {
	for(auto seg = street.getXsize() - 1; seg >= 0; --seg) {
		if(street(seg, 0) != -1) {
			long newSeg = seg + street(seg, 0);
			if(newSeg < street.getXsize()) {
				street(newSeg, 0) = street(seg, 0);
			}
			street(seg, 0) = -1;
		}
	}

//	Create a new car
	if(uniform01(randomEngine) < carGenerationRate && street(0, 0) == -1) {
		street(0, 0) = uniform05(randomEngine); //TODO random speed
	}
}

void simulateStreet(long segmentCount, double carDensity, double carGenerationRate, double p) {
	default_random_engine randomEngine(chrono::system_clock::now().time_since_epoch().count());
	uniform_real_distribution<double> uniform01(0., 1.);
	uniform_int_distribution<long> uniform05(0, 5);

	Grid<char> street = generateStreet(randomEngine, uniform01, uniform05, segmentCount, carDensity);

	cn::CImg<unsigned char> total = gridToImg(street);
	for(auto it = 0L; it < 500; ++it) {
		simulateStreetStep(randomEngine, uniform01, uniform05, street, carGenerationRate, p);

		cn::CImg<unsigned char> tmp = gridToImg(street);
		total = append(total, tmp);
	}
	cn::CImgDisplay disp(total, "Result", 1);
	while(! disp.is_closed()) {
		disp.wait();
	}
}

int main(int argc, char **argv) {
	long segmentCount = 300; // 2.25km
	double carDensity = 0.2;
	double carGenerationRate = 0.7;
	double p = 0.4;

	simulateStreet(segmentCount, carDensity, carGenerationRate, p);

	return EXIT_SUCCESS;
}
