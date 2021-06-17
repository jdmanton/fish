#include "CImg.h"

using namespace cimg_library;


namespace fish{
	double error_mse(const CImg<> &est, const CImg<> truth, const char* method) {
		double error_val = 0.0;
		cimg_forXY(est, x, y) {
			error_val += pow(abs(est(x, y) - truth(x, y)), 2);
		}
		return error_val / (truth.width() * truth.height());
	}

	double error_rmse(const CImg<> &est, const CImg<> truth, const char* method) {
		double error_val = 0.0;
		cimg_forXY(est, x, y) {
			error_val += pow(abs(est(x, y) - truth(x, y)), 2);
		}
		return sqrt(error_val / (truth.width() * truth.height()));
	}


	double error(const CImg<> &est, const CImg<> truth, const char* method) {
		double error_val = 0.0;

		int start_time = cimg::time();
		printf("\nCalculating error using ");
		if (!strcmp(method, "rmse")) {
			printf("root mean squared error method...");
			error_val = error_rmse(est, truth, method);
		} else if (!strcmp(method, "mse")) {
			printf("mean squared error method...");
			error_val = error_mse(est, truth, method);
		} else if (!strcmp(method, "psnr")) {
			printf("no");
		} else {
			printf("%s method not implemented.\n", method);
			exit(1);
		}
		int error_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", error_time);

		return error_val;
	}
}
