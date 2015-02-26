#pragma once

#include "CImg.h"
#include <vector>
#include <cmath>

using namespace cimg_library;

class VisualizationKreisverkehr {
	public:
		VisualizationKreisverkehr( long dimX, long dimY ) :
		seperationLine(dimX, 1, 1, 3) {
			firstAppend = true;
		}

		void appendRoundabout(StreetMap &sm) {
			CImg<unsigned char> streetMapImg = streetMapToImg(sm);

			if (firstAppend) {
				roundaboutImg = streetMapImg;
				firstAppend = false;
			}
			else {
				roundaboutImg.append(seperationLine, 'y', 0);
				roundaboutImg.append(streetMapImg, 'y', 0);
			}
		}

		void show() {
			CImgDisplay disp(roundaboutImg, "Result", 1);
			while(! disp.is_closed()) {
				disp.wait();
			}
		}

		void save() {
			roundaboutImg.save_png("roundabout_image.png", 1);
		}

	private:
		CImg<unsigned char> streetMapToImg(StreetMap &sm) {
			std::vector< std::vector<StreetSegment> > &streetSegments = sm.getContents();

			long dimX = streetSegments.size();
			long dimY = streetSegments[0].size();

			CImg streetMapImg( dimX, dimY, 1, 3 );

			for (long x = 0; x < dimX; ++x) {
				for (long y = 0; y < dimY; ++y) {
					StreetSegment s = streetSegments[x][y];
					// There is no street at this position -> gray
					if (s.isDummy()) {
						streetMapImg(x, y, 1, 0) = 100;
						streetMapImg(x, y, 1, 1) = 100;
						streetMapImg(x, y, 1, 2) = 100;
					}
					else
					{
						Vehicle* v = s.v;
						// There is no vehicle on the street -> black
						if (v == nullptr) {
							streetMapImg(x, y, 1, 0) = 0;
							streetMapImg(x, y, 1, 1) = 0;
							streetMapImg(x, y, 1, 2) = 0;
						}
						// There is a vehicle -> color corresponding to its speed
						else {
							long colorIntensity = long(765. * (double(v->currentSpeed) + 1.) / 8.);

							long red = min(255L, colorIntensity);
							colorIntensity -= red;
							long green = min(255L, colorIntensity);
							colorIntensity -= green;
							long blue =  min(255L, colorIntensity);

							streetMapImg(x,y,0,0) = (unsigned char) red;
							streetMapImg(x,y,0,1) = (unsigned char) green;
							streetMapImg(x,y,0,2) = (unsigned char) blue;
						}
					}
				}
			}

			return streetMapImg;
		}

		CImg<unsigned char> roundaboutImg;
		CImg<unsigned char> seperationLine;
		bool firstAppend;
};
