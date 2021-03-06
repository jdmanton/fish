#include "CImg.h"
#include <random>

using namespace cimg_library;

namespace fish{
	CImg<> scale(const CImg<> &raw, const float pin, const float pout) {
		float scale = pin / pout;
		int new_width = ceil(scale * raw.width());
		int new_height = ceil(scale * raw.height());
		CImg<> scaled(new_width, new_height, 1, 1, 0);

		printf("Scaled size:   %d x %d (scale factor = %f)\n", new_width, new_height, scale);

		std::default_random_engine generator;
		std::uniform_real_distribution<float> ddist(0.0, 1.0);

		int start_time = cimg::time();
		cimg_forXY(raw, x, y) {
			int photon_num = raw(x, y);
			for (int i = 0; i < photon_num; i++) {
				float randx = ddist(generator);
				float randy = ddist(generator);
				// We need to cast x, y to doubles to avoid rounding errors later leading to segfaults
				double px = ((double) x + randx) * scale;
				double py = ((double) y + randy) * scale;
				scaled(floor(px), floor(py)) += 1;
			}
		}
		int scale_time = cimg::time() - start_time;
		printf("Scaling time:  %d ms\n", scale_time);

		return scaled;
	}
}
