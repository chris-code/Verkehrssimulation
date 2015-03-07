#pragma once

#include "CImg.h"
#include <cmath>
#include <fstream>
#include <iostream> // FIXME remove this

using namespace cimg_library;

const long MAXSPEED = 7;

class VisualizationMultilane {
	public:
		VisualizationMultilane(long streetLength, long laneCount) :
			seperationLine(streetLength, 1, 1, 3, 100),
			speedCounter(streetLength, laneCount, 1, 1, 0),
			occupancyCounter(streetLength, laneCount, 1, 1, 0) {
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
			densities.push_back(r.computeDensity());
			iterations++;
		}

		void show() {
			CImgDisplay disp(roadImg, "Result", 1);
			while(! disp.is_closed()) {
				disp.wait();
			}
		}

		void save() {
			roadImg.save_png("output/multilane_image.png", 3);
			saveSpeedHeatMap();
			saveOccupancyHeatMap(true);
			saveOccupancyHeatMap(false);
			saveDensities();
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
					if (v != nullptr) {
//						speedCounter(s, l, 0, 0) = speedCounter(s, l, 0, 0) + v->currentSpeed;
						// update speed counter for each cell that was passed by the vehicle
						for (long pos = s; pos >= 0 && pos >= s - v->currentSpeed + 1; --pos) {
							speedCounter(pos, l, 0, 0) = speedCounter(pos, l, 0, 0) + v->currentSpeed;
						}
					}
				}
			}
		}

		void updateOccupancyHeatMap(Road &r) {
			for (auto s = 0; s < r.getStreetLength(); ++s) {
				for (auto l = 0; l < r.getLaneCount(); ++l) {
					Vehicle *v = r.getVehicle(s, l);
					if (v != nullptr) {
//						occupancyCounter(s, l, 0, 0) = occupancyCounter(s, l, 0, 0) + 1;
						// update speed counter for each cell that was passed by the vehicle
						for (long pos = s; pos >= 0 && pos >= s - v->currentSpeed + 1; --pos) {
							occupancyCounter(pos, l, 0, 0) = occupancyCounter(pos, l, 0, 0) + 1;
						}
					}
				}
			}
		}

		void saveSpeedHeatMap() {
			CImg<unsigned char> speedHeatMapColored(speedCounter.width(), speedCounter.height(), 1, 3);

			CImg<double> relativeSpeedMap(speedCounter.width(), speedCounter.height(), 1, 1);

			for (auto x = 0; x < speedHeatMapColored.width(); ++x) {
				for (auto y = 0; y < speedHeatMapColored.height(); y++) {
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

			speedHeatMapColored.save_png("output/multilane_speed_heat_map.png", 3);
		}

		void saveOccupancyHeatMap(bool relative) {
			CImg<unsigned char> occupancyHeatMapColored(occupancyCounter.width(), occupancyCounter.height(), 1, 3);

			long minValue = occupancyCounter.min();
			long maxValue = 765;
			if (!relative) {
				maxValue = long((double(occupancyCounter.max()) / double(iterations)) * 765.);
			}

			CImg<long> occupancyHeatMapNormalized = occupancyCounter.get_normalize(minValue, maxValue);

			for (auto x = 0; x < occupancyHeatMapColored.width(); ++x) {
				for (auto y = 0; y < occupancyHeatMapColored.height(); y++) {
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

			if (relative) {
				occupancyHeatMapColored.save_png("output/multilane_occupancy_heat_map_relative.png", 3);
			}
			else {
				occupancyHeatMapColored.save_png("output/multilane_occupancy_heat_map.png", 3);
			}
		}

		void saveDensities() {
			ofstream densitiesFile("output/multilane_densities.txt");

			double min = 1.;
			double max = 0.;

			for (long i = 0; i < long(densities.size()); ++i) {
				densitiesFile << densities[i];

				// save maximum
				if (densities[i] > max) {
					max = densities[i];
				}

				// save minimum
				if (densities[i] < min) {
					min = densities[i];
				}

				// new line
				if (i != (long(densities.size()) - 1)) {
					densitiesFile << "\n";
				}
			}

			densitiesFile.close();

			double minDist = 0.05;
			double deltaMinMax = max - min + minDist;

			double yRangeMin = std::max(0., (min - deltaMinMax));
			double yRangeMax = max + deltaMinMax;

			ofstream plotDensitiesFile("bin/multilane_plot_densities.txt");
			plotDensitiesFile << "set term png size 1024,768\n";
			plotDensitiesFile << "set output \"output/multilane_densities.png\"\n";
			plotDensitiesFile << "set title \"Traffic Density Multilane\" \n";
			plotDensitiesFile << "set xlabel \"Time (in s)\" \n";
			plotDensitiesFile << "set ylabel \"Traffic Density\" \n";
			plotDensitiesFile << "set yrange [" << yRangeMin << ":" << yRangeMax << "] \n";
			plotDensitiesFile << "plot \"output/multilane_densities.txt\" with lines\n";
			plotDensitiesFile.close();

//			system("gnuplot multilane_plot_densities.txt");
		}

		CImg<unsigned char> roadImg;
		CImg<unsigned char> seperationLine;
		CImg<long> speedCounter;
		CImg<long> occupancyCounter;
		bool firstAppend;
		std::vector<double> densities;
		long iterations;
};
