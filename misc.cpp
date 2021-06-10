#include "CImg.h"

using namespace cimg_library;

namespace fish {
	bool check_bounds(const CImg<> &img, int x, int y) {
		return (x >= 0 && x < img.width() && y >= 0 && y < img.height()) ? true : false;
	}
}