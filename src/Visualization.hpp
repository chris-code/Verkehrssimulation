#pragma once

#include "CImg.h"

#include <iostream>

using namespace cimg_library;

class Visualization {
	public:
		Visualization(long streetLength) : seperationLine(streetLength, 1, 1, 3) {
			firstAppend = true;
			for (auto x = 0; x < streetLength; ++x) {
				seperationLine(x,0,0,2) = 255;
			}
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

	private:
		CImg<unsigned char> roadToImg(Road &r) {
			CImg<unsigned char> img(r.getStreetLength(), r.getLaneCount(), 1, 3);

			for (auto s = 0; s < r.getStreetLength(); ++s) {
				for (auto l = 0; l < r.getLaneCount(); ++l) {
					Vehicle *v = r.getVehicle(s, l);
					if (v != nullptr) {
						switch (v->currentSpeed) {
							// white
							case 0:
								img(s,l,0,0) = 255;
								img(s,l,0,1) = 255;
								img(s,l,0,2) = 255;
								break;
							// bright yellow
							case 1:
								img(s,l,0,0) = 255;
								img(s,l,0,1) = 255;
								img(s,l,0,2) = 126;
								break;
							// yellow
							case 2:
								img(s,l,0,0) = 255;
								img(s,l,0,1) = 255;
								img(s,l,0,2) = 0;
								break;
							// bright orange
							case 3:
								img(s,l,0,0) = 255;
								img(s,l,0,1) = 170;
								img(s,l,0,2) = 0;
								break;
							// orange
							case 4:
								img(s,l,0,0) = 255;
								img(s,l,0,1) = 85;
								img(s,l,0,2) = 0;
								break;
							// red
							case 5:
								img(s,l,0,0) = 255;
								img(s,l,0,1) = 0;
								img(s,l,0,2) = 0;
								break;
							// dark red
							case 6:
								img(s,l,0,0) = 170;
								img(s,l,0,1) = 0;
								img(s,l,0,2) = 0;
								break;
							// brown
							case 7:
								img(s,l,0,0) = 85;
								img(s,l,0,1) = 0;
								img(s,l,0,2) = 0;
								break;
							default:
								img(s,l,0,0) = 0;
								img(s,l,0,1) = 0;
								img(s,l,0,2) = 0;
						}
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
