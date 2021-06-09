#include "CImg.h"
#include "fish.h"
#include <random>
#include <cassert>

using namespace cimg_library;

namespace fish{
	CImg<> rotate_coord(const CImg<> &raw, const float angle) {
		CImg<> rotated(raw.width(), raw.height(), 1, 1, 0);
		std::default_random_engine generator;
		std::uniform_real_distribution<float> ddist(0.0, 1.0);

		const double theta = -angle * M_PI / 180;
		const double centre_x = raw.width() / 2.0;
		const double centre_y = raw.height() / 2.0;

		cimg_forXY(raw, x, y) {
			int photon_num = raw(x, y);
			for (int i = 0; i < photon_num; i++) {
				double xpos = ((double) x - centre_x) + ddist(generator);
				double ypos = ((double) y - centre_y) + ddist(generator);
				double new_x = std::cos(theta) * xpos - std::sin(theta) * ypos + centre_x;
				double new_y = std::sin(theta) * xpos + std::cos(theta) * ypos + centre_y;
				int px = floor(new_x);
				int py = floor(new_y);
				if (fish::check_bounds(rotated, px, py)) {
					rotated(px, py) += 1;
				}
			}
		}
		
		return rotated;
	}


	CImg<> rotate(const CImg<> &raw, const float angle) {
		CImg<> rotated;

		int start_time = cimg::time();
		printf("\nRotating image using coord draw method...");
		fflush(stdout);
		rotated = rotate_coord(raw, angle);
		int rotation_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", rotation_time);

		return rotated;
	}
}
