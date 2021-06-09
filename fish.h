#include "CImg.h"
using namespace cimg_library;

namespace fish {
	CImg<> affine(const CImg<> &raw, const float affmat[16]);
	CImg<> intensify(const CImg<> &raw, const float scale);
	CImg<> rotate(const CImg<> &raw, const float angle);
	CImg<> scale(const CImg<> &raw, const float pin, const float pout);
	CImg<> translate(const CImg<> &raw, const float x_shift, const float y_shift, const char* method);
	CImg<> load_tiff(const char* filename);
	void save_tiff(CImg<> &img, const char* filename, float pitch_xy, float spacing_z);
	bool check_bounds(const CImg<> &img, int x, int y);
}
