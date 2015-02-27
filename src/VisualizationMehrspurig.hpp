#pragma once

#include "CImg.h"
#include <cmath>
#include <iostream> // FIXME remove this

using namespace cimg_library;

class VisualizationMehrspurig {
	public:
		VisualizationMehrspurig(long streetLength, long laneCount) :
			seperationLine(streetLength, 1, 1, 3, 100),
			speedHeatMap(streetLength, laneCount, 1, 1, 0),
			occupancyHeatMap(streetLength, laneCount, 1, 1, 0) {
			firstAppend = true;
			iterations = 0;
		}

		void appendRoad(Road &r) {
			CImg<unsigned char> rImg = roadToImg(r);
			if (firstAppend) {
				roadImg = rImg;
				firstAppend = false;
			}
			else {
				roadImg.append(seperationLine, 'y', 0);
				roadImg.append(rImg, 'y', 0);
			}

			updateSpeedHeatMap(r);
			updateOccupancyHeatMap(r);

			iterations++;
		}

		void show() {
			CImgDisplay disp(roadImg, "Result", 1);
			while(! disp.is_closed()) {
				disp.wait();
			}
		}

		void save() {
			roadImg.save_png("road_image.png", 3);
			saveSpeedHeatMap();
			saveOccupancyHeatMap();
		}

	private:
		CImg<unsigned char> roadToImg(Road &r) {
			CImg<unsigned char> img(r.getStreetLength(), r.getLaneCount(), 1, 3);

			for (auto s = 0; s < r.getStreetLength(); ++s) {
				for (auto l = 0; l < r.getLaneCount(); ++l) {
					Vehicle *v = r.getVehicle(s, l);
					if (v != nullptr) {
						long colorIntensity = long(765. * (double(v->currentSpeed) + 1.) / 8.);

						long red = min(255L, colorIntensity);
						colorIntensity -= red;
						long green = min(255L, colorIntensity);
						colorIntensity -= green;
						long blue =  min(255L, colorIntensity);

						img(s,l,0,0) = (unsigned char) red;
						img(s,l,0,1) = (unsigned char) green;
						img(s,l,0,2) = (unsigned char) blue;
					}
					else {
						// black
						img(s,l,0,0) = 0;
						img(s,l,0,1) = 0;
						img(s,l,0,2) = 0;
					}

				}
			}

			return img;
		}

		void updateSpeedHeatMap(Road &r) {
			for (auto s = 0; s < r.getStreetLength(); ++s) {
				for (auto l = 0; l < r.getLaneCount(); ++l) {
					Vehicle *v = r.getVehicle(s, l);
					if (v == nullptr) {
						speedHeatMap(s, l, 0, 0) = speedHeatMap(s, l, 0, 0) + 7; // TODO Replace "8" by MaxSpeed Constant
					}
					else {
						speedHeatMap(s, l, 0, 0) = speedHeatMap(s, l, 0, 0) + v->currentSpeed;
					}
				}
			}
		}

		void updateOccupancyHeatMap(Road &r) {
			for (auto s = 0; s < r.getStreetLength(); ++s) {
				for (auto l = 0; l < r.getLaneCount(); ++l) {
					Vehicle *v = r.getVehicle(s, l);
					if (v != nullptr) {
						occupancyHeatMap(s, l, 0, 0) = occupancyHeatMap(s, l, 0, 0) + 1;
					}
				}
			}
		}

		void saveSpeedHeatMap() {
			CImg<unsigned char> speedHeatMapColored(speedHeatMap.width(), speedHeatMap.height(), 1, 3);

			long minValue = long(765. * ((double(speedHeatMap.min()) / double(iterations)) + 1.) / 8.);
			long maxValue = long(765. * ((double(speedHeatMap.max()) / double(iterations)) + 1.) / 8.); // TODO Replace 8 by MaxSpeed Constant + 1

			CImg<long> speedHeatMapNormalized0_765 = speedHeatMap.get_normalize(minValue, maxValue);

			for (auto x = 0; x < speedHeatMapColored.width(); ++x) {
				for (auto y = 0; y < speedHeatMapColored.height(); y++) {
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

			speedHeatMapColored.save_png("multilane_speed_heat_map.png", 3);
		}

		void saveOccupancyHeatMap() {
			CImg<unsigned char> occupancyHeatMapColored(occupancyHeatMap.width(), occupancyHeatMap.height(), 1, 3);

			CImg<long> occupancyHeatMapNormalized0_765 = occupancyHeatMap.get_normalize(0, 765);

			for (auto x = 0; x < occupancyHeatMapColored.width(); ++x) {
				for (auto y = 0; y < occupancyHeatMapColored.height(); y++) {
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

			occupancyHeatMapColored.save_png("multilane_occupancy_heat_map.png", 3);
		}

		CImg<unsigned char> roadImg;
		CImg<unsigned char> seperationLine;
		CImg<long> speedHeatMap;
		CImg<long> occupancyHeatMap;
		bool firstAppend;
		long iterations;
};
