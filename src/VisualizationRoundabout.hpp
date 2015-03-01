#pragma once

#include "CImg.h"
#include <vector>
#include <cmath>

using namespace cimg_library;

const long MAXSPEED = 7;

class VisualizationRoundabout {
	public:
		VisualizationRoundabout( long dimX, long dimY ) :
			seperationLine(dimX, 1, 1, 3, 100),
			speedCounter(dimX, dimY, 1, 1, 0),
			occupancyCounter(dimX, dimY, 1, 1, 0) {
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
						if (s->v != nullptr) {
							speedCounter(x,y,0,0) = speedCounter(x,y,0,0) + s->v->currentSpeed;
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
							occupancyCounter(x,y,0,0) = occupancyCounter(x,y,0,0) + 1;
						}
					}
				}
			}
		}

		void saveSpeedHeatMap() {
			CImg<unsigned char> speedHeatMapColored(speedCounter.width(), speedCounter.height(), 1, 3);

			std::vector< std::vector<StreetSegment> > &streetSegments = lastUsedStreetMap->getContents();

			CImg<double> relativeSpeedMap(speedCounter.width(), speedCounter.height(), 1, 1);

			for (auto x = 0; x < speedHeatMapColored.width(); ++x) {
				for (auto y = 0; y < speedHeatMapColored.height(); y++) {
					if (streetSegments[x][y].isDummy()) {
						speedHeatMapColored(x, y, 0, 0) = 100;
						speedHeatMapColored(x, y, 0, 1) = 100;
						speedHeatMapColored(x, y, 0, 2) = 100;
					}
					else {
						// calculate relative speed at position (x,y)
						if (occupancyCounter(x, y, 0, 0) == 0) {
							relativeSpeedMap(x, y, 0, 0) = 0.;
						}
						else {
							relativeSpeedMap(x, y, 0, 0) = double(speedCounter(x, y, 0, 0)) / double(occupancyCounter(x, y, 0, 0));
						}

						long colorIntensity = 0;
						if (occupancyCounter(x, y, 0, 0) != 0) {
							colorIntensity = long(((relativeSpeedMap(x, y, 0, 0) + 1.) / (double(MAXSPEED) + 1.)) * 765.);
						}

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
			CImg<unsigned char> occupancyHeatMapColored(occupancyCounter.width(), occupancyCounter.height(), 1, 3);

			long minValue = occupancyCounter.min();
			long maxValue = 765;

			CImg<long> occupancyHeatMapNormalized = occupancyCounter.get_normalize(minValue, maxValue);

			std::vector< std::vector<StreetSegment> > &streetSegments = lastUsedStreetMap->getContents();

			for (auto x = 0; x < occupancyHeatMapColored.width(); ++x) {
				for (auto y = 0; y < occupancyHeatMapColored.height(); y++) {
					if (streetSegments[x][y].isDummy()) {
						occupancyHeatMapColored(x, y, 0, 0) = 100;
						occupancyHeatMapColored(x, y, 0, 1) = 100;
						occupancyHeatMapColored(x, y, 0, 2) = 100;
					}
					else {
						long colorIntensity = occupancyHeatMapNormalized(x, y, 0, 0);
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
		CImg<long> speedCounter;
		CImg<long> occupancyCounter;
		StreetMap *lastUsedStreetMap;
		bool firstAppend;
		long iterations;
};
