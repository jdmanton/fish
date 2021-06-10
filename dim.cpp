#include "CImg.h"
#include <random>
#include <cmath>

using namespace cimg_library;


namespace fish{
	CImg<> dim_binom(const CImg<> &raw, const float scale) {
		CImg<> dimmed(raw.width(), raw.height(), 1, 1, 0);
		std::default_random_engine generator;

		if (scale > 1.0) {
			printf("\nScale > 1.0 not supported. Please use intensify instead.\n");
			exit(1);
		}

		cimg_forXY(raw, x, y) {
			int photon_num = raw(x, y);
			std::binomial_distribution<> ddist(photon_num, scale);
			dimmed(x, y) = ddist(generator);
		}
		
		return dimmed;
	}


	CImg<> dim(const CImg<> &raw, const float scale) {
		CImg<> dimmed;

		int start_time = cimg::time();
		printf("\nDimming image using binomial method...");
		fflush(stdout);
		dimmed = dim_binom(raw, scale);
		int intensification_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", intensification_time);

		return dimmed;
	}
}
