#pragma once

#include "CImg.h"
#include <vector>
#include <cmath>

using namespace cimg_library;

class VisualizationKreisverkehr {
	public:
		VisualizationKreisverkehr( long dimX, long dimY ) :
			seperationLine(dimX, 1, 1, 3, 100),
			speedHeatMap(dimX, dimY, 1, 1, 0),
			occupancyHeatMap(dimX, dimY, 1, 1, 0) {
			firstAppend = true;
			lastUsedStreetMap = nullptr;
			iterations = 0;
		}

		void appendRoundabout(StreetMap &sm) {
			CImg<unsigned char> streetMapImg = streetMapToImg(sm);

			lastUsedStreetMap = &sm;

			if (firstAppend) {
				roundaboutImg = streetMapImg;
				firstAppend = false;
			}
			else {
				roundaboutImg.append(seperationLine, 'y', 0);
				roundaboutImg.append(streetMapImg, 'y', 0);
			}

			updateSpeedHeatMap(sm);
			updateOccupancyHeatMap(sm);

			iterations++;
		}

		void show() {
			CImgDisplay disp(roundaboutImg, "Result", 1);
			while(! disp.is_closed()) {
				disp.wait();
			}
		}

		void save() {
			roundaboutImg.save_png("roundabout_image.png", 3);
			saveSpeedHeatMap();
			saveOccupancyHeatMap();
		}

	private:
		CImg<unsigned char> streetMapToImg(StreetMap &sm) {
			std::vector< std::vector<StreetSegment> > &streetSegments = sm.getContents();

			long dimX = streetSegments.size();
			long dimY = streetSegments[0].size();

			CImg<unsigned char> streetMapImg( dimX, dimY, 1, 3 );

			for (long x = 0; x < dimX; ++x) {
				for (long y = 0; y < dimY; ++y) {
					StreetSegment *s = &streetSegments[x][y];
					// There is no street at this position -> gray
					if (s->isDummy()) {
						streetMapImg(x, y, 0, 0) = 100;
						streetMapImg(x, y, 0, 1) = 100;
						streetMapImg(x, y, 0, 2) = 100;
					}
					else
					{
						// There is no vehicle on the street -> black
						if (s->v == nullptr) {
							streetMapImg(x, y, 0, 0) = 0;
							streetMapImg(x, y, 0, 1) = 0;
							streetMapImg(x, y, 0, 2) = 0;
						}
						// There is a vehicle -> color corresponding to its speed
						else {
							long colorIntensity = long(765. * (double(s->v->currentSpeed) + 1.) / 8.);

							long red = min(255L, colorIntensity);
							colorIntensity -= red;
							long green = min(255L, colorIntensity);
							colorIntensity -= green;
							long blue =  min(255L, colorIntensity);

							streetMapImg(x, y, 0, 0) = (unsigned char) red;
							streetMapImg(x, y, 0, 1) = (unsigned char) green;
							streetMapImg(x, y, 0, 2) = (unsigned char) blue;
						}
					}
				}
			}

			return streetMapImg;
		}

		void updateSpeedHeatMap(StreetMap &sm) {
			std::vector< std::vector<StreetSegment> > &streetSegments = sm.getContents();

			long dimX = streetSegments.size();
			long dimY = streetSegments[0].size();

			for (long x = 0; x < dimX; ++x) {
				for (long y = 0; y < dimY; ++y) {
					StreetSegment *s = &streetSegments[x][y];
					if (!s->isDummy()) {
						// If there is no car at this position, we consider the speed in this cell as "high"
						if (s->v == nullptr) {
							speedHeatMap(x,y,0,0) = speedHeatMap(x,y,0,0) + 4; // TODO Replace "4" by MaxSpeed variable or constant
						}
						else {
							speedHeatMap(x,y,0,0) = speedHeatMap(x,y,0,0) + s->v->currentSpeed;
						}
					}
				}
			}
		}

		void updateOccupancyHeatMap(StreetMap &sm) {
			std::vector< std::vector<StreetSegment> > &streetSegments = sm.getContents();

			long dimX = streetSegments.size();
			long dimY = streetSegments[0].size();

			for (long x = 0; x < dimX; ++x) {
				for (long y = 0; y < dimY; ++y) {
					StreetSegment *s = &streetSegments[x][y];
					if (!s->isDummy()) {
						if (s->v != nullptr) {
							occupancyHeatMap(x,y,0,0) = occupancyHeatMap(x,y,0,0) + 1;
						}
					}
				}
			}
		}

		void saveSpeedHeatMap() {
			CImg<unsigned char> speedHeatMapColored(speedHeatMap.width(), speedHeatMap.height(), 1, 3);

			long minValue = long(765. * ((double(speedHeatMap.min()) / double(iterations)) + 1.) / 5.);
			long maxValue = long(765. * ((double(speedHeatMap.max()) / double(iterations)) + 1.) / 5.); // TODO Replace 5 by MaxSpeed Constant + 1
			// Reminder: maxValue has been 765 * 4 / 8 before I calculated maxValue

			CImg<long> speedHeatMapNormalized0_765 = speedHeatMap.get_normalize(minValue, maxValue); // TODO Replace "4" by MaxSpeed constant
			// Here we take 765 * 4 / 8 as maximal value, because the minimal speed is 0 (dark red) and the maximal speed is 4 (yellow).
			// We don't want speed 4 to be white, so the maximal value must be 4/8 times 765.

			std::vector< std::vector<StreetSegment> > &streetSegments = lastUsedStreetMap->getContents();

			for (auto x = 0; x < speedHeatMapColored.width(); ++x) {
				for (auto y = 0; y < speedHeatMapColored.height(); y++) {
					if (streetSegments[x][y].isDummy()) {
						speedHeatMapColored(x, y, 0, 0) = 100;
						speedHeatMapColored(x, y, 0, 1) = 100;
						speedHeatMapColored(x, y, 0, 2) = 100;
					}
					else {
						long colorIntensity = speedHeatMapNormalized0_765(x, y, 0, 0);
						long red = min(255L, colorIntensity);
						colorIntensity -= red;
						long green = min(255L, colorIntensity);
						colorIntensity -= green;
						long blue =  min(255L, colorIntensity);

						speedHeatMapColored(x ,y, 0, 0) = (unsigned char) red;
						speedHeatMapColored(x ,y, 0, 1) = (unsigned char) green;
						speedHeatMapColored(x ,y, 0, 2) = (unsigned char) blue;
					}
				}
			}

			speedHeatMapColored.save_png("roundabout_speed_heat_map.png", 3);
		}

		void saveOccupancyHeatMap() {
			CImg<unsigned char> occupancyHeatMapColored(occupancyHeatMap.width(), occupancyHeatMap.height(), 1, 3);

			CImg<long> occupancyHeatMapNormalized0_765 = occupancyHeatMap.get_normalize(0, 765);

			std::vector< std::vector<StreetSegment> > &streetSegments = lastUsedStreetMap->getContents();

			for (auto x = 0; x < occupancyHeatMapColored.width(); ++x) {
				for (auto y = 0; y < occupancyHeatMapColored.height(); y++) {
					if (streetSegments[x][y].isDummy()) {
						occupancyHeatMapColored(x, y, 0, 0) = 100;
						occupancyHeatMapColored(x, y, 0, 1) = 100;
						occupancyHeatMapColored(x, y, 0, 2) = 100;
					}
					else {
						long colorIntensity = occupancyHeatMapNormalized0_765(x, y, 0, 0);
						long red = min(255L, colorIntensity);
						colorIntensity -= red;
						long green = min(255L, colorIntensity);
						colorIntensity -= green;
						long blue =  min(255L, colorIntensity);

						occupancyHeatMapColored(x ,y, 0, 0) = (unsigned char) red;
						occupancyHeatMapColored(x ,y, 0, 1) = (unsigned char) green;
						occupancyHeatMapColored(x ,y, 0, 2) = (unsigned char) blue;
					}
				}
			}

			occupancyHeatMapColored.save_png("roundabout_occupancy_heat_map.png", 3);
		}

		CImg<unsigned char> roundaboutImg;
		CImg<unsigned char> seperationLine;
		CImg<long> speedHeatMap;
		CImg<long> occupancyHeatMap;
		StreetMap *lastUsedStreetMap;
		bool firstAppend;
		long iterations;
};
