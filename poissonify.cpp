#include "CImg.h"
#include <random>

using namespace cimg_library;


namespace fish{
	CImg<> poissonify(const CImg<> &raw, const float scale) {
		CImg<> poissonified(raw.width(), raw.height(), 1, 1, 0);
		std::default_random_engine generator;

		int start_time = cimg::time();
		printf("\nPoissonifying image...");
		fflush(stdout);

		cimg_forXY(raw, x, y) {
			std::poisson_distribution<> pdist(round(raw(x, y) * scale));
			poissonified(x, y) = pdist(generator);
		}
		
		int poissonification_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", poissonification_time);

		return poissonified;
	}
}
