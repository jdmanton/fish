#include "CImg.h"
#include "fish.h"
#include <random>
#include <cassert>

using namespace cimg_library;

namespace fish{
	CImg<> translate_coord(const CImg<> &raw, const float shift_x, const float shift_y) {
		CImg<> translated(raw.width(), raw.height(), 1, 1, 0);
		std::default_random_engine generator;
		std::uniform_real_distribution<float> ddist(0.0, 1.0);

		cimg_forXY(raw, x, y) {
			int photon_num = raw(x, y);
			for (int i = 0; i < photon_num; i++) {
				double new_x = (double) x + shift_x + ddist(generator);
				double new_y = (double) y + shift_y + ddist(generator);
				int px = floor(new_x);
				int py = floor(new_y);
				if (px >= 0 && px < raw.width() && py >= 0 && py < raw.height()) {
					translated(px, py) += 1;
				}
			}
		}
		
		return translated;
	}

	
	CImg<> translate_binom(const CImg<> &raw, const float shift_x, const float shift_y) {
		CImg<> translated(raw.width(), raw.height(), 1, 1, 0);
		std::default_random_engine generator;

		float further_x_weight = shift_x - floor(shift_x);
		float further_y_weight = shift_y - floor(shift_y);

		cimg_forXY(raw, x, y) {
			int photon_num = raw(x, y);
			std::binomial_distribution<> xdist(photon_num, further_x_weight);
			int further_x_num = xdist(generator);
			int nearer_x_num = photon_num - further_x_num;
			std::binomial_distribution<> fxydist(further_x_num, further_y_weight);
			std::binomial_distribution<> nxydist(nearer_x_num, further_y_weight);
			
			int fxfy = fxydist(generator);
			int fxny = further_x_num - fxfy;
			int nxfy = nxydist(generator);
			int nxny = nearer_x_num - nxfy;
			assert(fxfy + fxny + nxfy + nxny == photon_num);

			int further_x = x + ceil(shift_x);
			int further_y = y + ceil(shift_y);			
			translated(x, y) += nxny;
			if (fish::check_bounds(raw, 0, further_y)) translated(x, further_y) += nxfy;
			if (fish::check_bounds(raw, further_x, 0)) translated(further_x, y) += fxny;
			if (fish::check_bounds(raw, further_x, further_y)) translated(further_x, further_y) += fxfy;
		}

		return translated;
	}


	CImg<> translate(const CImg<> &raw, const float shift_x, const float shift_y, const char* method) {
		CImg<> translated;

		int start_time = cimg::time();
		printf("\nTranslating image using ");
		if (!strcmp(method, "coord")) {
			printf("coord draw method...");
			fflush(stdout);
			translated = translate_coord(raw, shift_x, shift_y);
		} else {
			printf("binomial method...");
			fflush(stdout);
			translated = translate_binom(raw, shift_x, shift_y);
		}
		int translation_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", translation_time);

		return translated;
	}
}
