#include "CImg.h"
#include <random>
#include <cmath>

using namespace cimg_library;

int C(int n, int k) {
    double res = 1;
    for (int i = 1; i <= k; ++i)
        res = res * (n - k + i) / i;
    return (int)(res + 0.01);
}

double binomial(int N, int x, float p) {
	C(N, x) * pow(p, x) * pow(1 - p, N - x);
}


namespace fish{
	CImg<> intensify_binom(const CImg<> &raw, const float scale) {
		CImg<> intensified(raw.width(), raw.height(), 1, 1, 0);
		std::default_random_engine generator;

		if (scale > 1.0) {
			cimg_forXY(raw, x, y) {
				// Maximise N over plausible range
				int photon_num = raw(x, y);
				int max_num = scale * photon_num;
				int max_N = 0;
				for (int i = 1; i <= max_num; i++) {
					max_N = (binomial(i, photon_num, 1.0 / (1.0 - scale)) > max_N) ? i : max_N;
				}
				std::poisson_distribution<> pdist(max_N);
				intensified(x, y) = pdist(generator);
			}
		} else {
			cimg_forXY(raw, x, y) {
				int photon_num = raw(x, y);
				std::binomial_distribution<> ddist(photon_num, scale);
				intensified(x, y) = ddist(generator);
			}
		}
		
		return intensified;
	}


	CImg<> intensify(const CImg<> &raw, const float scale, const char* method) {
		CImg<> intensified;

		int start_time = cimg::time();
		printf("\nIntensifying image using binomial method...");
		fflush(stdout);
		intensified = intensify_binom(raw, scale);
		int intensification_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", intensification_time);

		return intensified;
	}
}
