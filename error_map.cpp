#include "CImg.h"

using namespace cimg_library;


namespace fish{
	CImg<> error_map_diff(const CImg<> &est, const CImg<> truth, const char* method) {
		CImg<> errors(truth.width(), truth.height(), 1, 1, 0);
		
		cimg_forXY(errors, x, y) {
			errors(x, y) = est(x, y) - truth(x, y);
		}
		return errors;
	}


	CImg<> error_map(const CImg<> &est, const CImg<> truth, const char* method) {
		CImg<> errors;

		int start_time = cimg::time();
		printf("\nCalculating error using ");
		if (!strcmp(method, "diff")) {
			printf("pixel differences method...");
			errors = error_map_diff(est, truth, method);
		} else {
			printf("%s method not implemented.\n", method);
			exit(1);
		}
		int error_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", error_time);

		return errors;
	}
}
