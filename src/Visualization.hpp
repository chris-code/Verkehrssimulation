#pragma once

#include "CImg.h"
#include <cmath>

using namespace cimg_library;

class Visualization {
	public:
		Visualization(long streetLength) : seperationLine(streetLength, 1, 1, 3, 100) {
			firstAppend = true;
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
		}

		void show() {
			CImgDisplay disp(roadImg, "Result", 1);
			while(! disp.is_closed()) {
				disp.wait();
			}
		}

		void save() {
			roadImg.save_png("road_image.png", 1);
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

		CImg<unsigned char> roadImg;
		CImg<unsigned char> seperationLine;
		bool firstAppend;
};
