#include "CImg.h"
using namespace cimg_library;

namespace fish {
	CImg<> affine(const CImg<> &raw, const float affmat[16]);
	CImg<> dim(const CImg<> &raw, const float scale);
	CImg<> error_map(const CImg<> &est, const CImg<> truth, const char* method);
	CImg<> intensify(const CImg<> &raw, const float scale);
	CImg<> poissonify(const CImg<> &raw, const float scale);
	CImg<> rebin(const CImg<> &raw, const int scale, const char* direction);
	CImg<> rotate(const CImg<> &raw, const float angle, const char* method);
	CImg<> scale(const CImg<> &raw, const float pin, const float pout);
	CImgList<> split(const CImg<> &raw, const float p1);
	CImg<> translate(const CImg<> &raw, const float x_shift, const float y_shift, const char* method);
	CImg<> load_tiff(const char* filename);
	double error(const CImg<> &est, const CImg<> truth, const char* method);
	void save_tiff(CImg<> &img, const char* filename, float pitch_xy, float spacing_z);
	bool check_bounds(const CImg<> &img, int x, int y);
}
