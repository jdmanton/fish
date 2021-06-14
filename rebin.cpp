#include "CImg.h"
#include "fish.h"
#include <random>
#include <cassert>
#include <vector>

using namespace cimg_library;

namespace fish{
	CImg<> rebin_up(const CImg<> &raw, const int scale) {
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);

		std::default_random_engine generator;
		std::uniform_int_distribution<int> ddist(1, scale * scale);

		cimg_forXY(raw, x, y) {
			int photon_num = raw(x, y);
			for (int i = 0; i < photon_num; i++) {
				int bin = ddist(generator);
				int ind = bin - 1;
				int yb = ind / scale;
				int xb = bin % scale;
				int xs = x * scale + xb;
				int ys = y * scale + yb;
				scaled(xs, ys) += 1;
			}
		}

		return scaled;
	}


	CImg<> rebin_down(const CImg<> &raw, const int scale) {
		CImg<> scaled(raw.width() / scale, raw.height() / scale, 1, 1, 0);

		cimg_forXY(raw, x, y) {
			int xr = x / scale;
			int yr = y / scale;
			scaled(xr, yr) += raw(x, y);
		}

		return scaled;
	}


	CImg<> rebin_up_nn(const CImg<> &raw, const int scale) {
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);

		cimg_forXY(scaled, x, y) {
			int xr = x / scale;
			int yr = y / scale;
			scaled(x, y) = raw(xr, yr) / (scale * scale);
		}

		return scaled;
	}


	CImg<> rebin_condense(const CImg<> &raw, const int scale) {
		// 1. Collect together all the photons and throw them across the upscaled image
		// 2. Use the original image to work out which areas have too few/many photons
		// 3. Redistribute the photons in too bright areas to dim areas

		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);

		std::default_random_engine generator;

		// Step 1
		int total_photons = raw.sum();
		std::uniform_int_distribution<int> ddist(0, scaled.width() * scaled.height() - 1);
		for (int i = 0; i < total_photons; i++) {
			int ind = ddist(generator);
			int xb = ind % scaled.width();
			int yb = ind / scaled.height();
			scaled(xb, yb) += 1;
		}

		// Step 2
		CImg<> errors = fish::rebin_down(scaled, scale) - raw;
		std::vector<int> deficiencies_x;
		std::vector<int> deficiencies_y;
		std::vector<int> deficiencies_n;
		cimg_forXY(errors, x, y) {
			if (errors(x, y) < 0) {
				deficiencies_x.push_back(x);
				deficiencies_y.push_back(y);
				deficiencies_n.push_back(-errors(x, y));
			}
		}
		cimg_forXY(errors, x, y) {
			if (errors(x, y) <= 0) {
				continue;
			} else {
				int photon_num = errors(x, y);
				for (int i = 0; i < photon_num; i++) {
					// printf("%d pixels left to fill.\n", deficiencies_n.size());
					std::uniform_int_distribution<int> ddist(0, deficiencies_n.size() - 1);
					int ind = ddist(generator);
					deficiencies_n[ind] -= 1;

					int nx = deficiencies_x[ind];
					int ny = deficiencies_y[ind];
					
					std::uniform_int_distribution<int> pdist(0, scale - 1);
					
					int del_x = scale * x + pdist(generator);
					int del_y = scale * y + pdist(generator);
					int add_x = scale * nx + pdist(generator);
					int add_y = scale * ny + pdist(generator);
					
					// printf("Moving photon from (%d, %d) to (%d, %d).\n", del_x, del_y, add_x, add_y);

					scaled(del_x, del_y) -= 1;
					scaled(add_x, add_y) += 1;

					if (deficiencies_n[ind] == 0) {
						deficiencies_n.erase(deficiencies_n.begin() + ind);
						deficiencies_x.erase(deficiencies_x.begin() + ind);
						deficiencies_y.erase(deficiencies_y.begin() + ind);
					}
				}
			}
		}

		return scaled;
	}


	CImg<> rebin_condense2(const CImg<> &raw, const int scale) {
		// 1. Collect together all the photons and throw them across the upscaled image
		// 2. Use the original image to work out which areas have too few/many photons
		// 3. Redistribute the photons in too bright areas to dim areas

		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);

		std::default_random_engine generator;

		// Step 1
		int total_photons = raw.sum();
		std::uniform_int_distribution<int> ddist(0, scaled.width() * scaled.height() - 1);
		for (int i = 0; i < total_photons; i++) {
			int ind = ddist(generator);
			int xb = ind % scaled.width();
			int yb = ind / scaled.height();
			scaled(xb, yb) += 1;
		}

		// Step 2
		CImg<> errors = fish::rebin_down(scaled, scale) - raw;
		std::vector<int> deficiencies_x;
		std::vector<int> deficiencies_y;
		std::vector<int> extras_x;
		std::vector<int> extras_y;
		int num_to_fix = 0;
		cimg_forXY(errors, x, y) {
			if (errors(x, y) < 0) {
				for (int i = 0; i < -errors(x, y); i++) {
					deficiencies_x.push_back(x);
					deficiencies_y.push_back(y);
				}
			} else if (errors(x, y) > 0) {
				for (int i = 0; i < errors(x, y); i++) {
					extras_x.push_back(x);
					extras_y.push_back(y);
					++num_to_fix;
				}
			}
		}

		for (int i = 0; i < num_to_fix; i++) {
			std::uniform_int_distribution<int> edist(0, extras_x.size() - 1);
			int e_ind = edist(generator);
			std::uniform_int_distribution<int> ddist(0, deficiencies_x.size() - 1);
			int d_ind = ddist(generator);

			std::uniform_int_distribution<int> pdist(0, scale - 1);
					
			int del_x = scale * extras_x[e_ind] + pdist(generator);
			int del_y = scale * extras_y[e_ind] + pdist(generator);
			int add_x = scale * deficiencies_x[d_ind] + pdist(generator);
			int add_y = scale * deficiencies_y[d_ind] + pdist(generator);
			
			// printf("Moving photon from (%d, %d) to (%d, %d).\n", del_x, del_y, add_x, add_y);
			// fflush(stdout);

			scaled(del_x, del_y) -= 1;
			scaled(add_x, add_y) += 1;
			// printf("Photon reassignment complete.\n");
			// fflush(stdout);

			extras_x.erase(extras_x.begin() + e_ind);
			extras_y.erase(extras_y.begin() + e_ind);
			deficiencies_x.erase(deficiencies_x.begin() + d_ind);
			deficiencies_y.erase(deficiencies_y.begin() + d_ind);
		}

		return scaled;
	}


	CImg<> rebin_fourier(const CImg<> &raw, const int scale) {
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);
		
		// FT and fftshift
		CImgList<> f = raw.get_FFT();
		f[0].shift(raw.width() / 2, raw.height() / 2, 0, 0, 2);
		f[1].shift(raw.width() / 2, raw.height() / 2, 0, 0, 2);

		// Pad FT
		CImgList<> f_pad = CImgList<>(2, raw.width() * scale, raw.height() * scale, 1, 1, 0);
		int pad_x = raw.width() * (scale - 1);
		int pad_y = raw.height() * (scale - 1);
		int x0 = pad_x / 2;
		int y0 = pad_y / 2;
		f_pad[0].draw_image(x0, y0, 0, 0, f[0]);
		f_pad[1].draw_image(x0, y0, 0, 0, f[1]);

		// Inverse transform
		f_pad.FFT(true);
		cimg_forXY(scaled, x, y) {
			scaled(x, y) = sqrt(f_pad[0](x, y) * f_pad[0](x, y) + f_pad[1](x, y) * f_pad[1](x, y));
		}

		return scaled;
	}


	CImg<> rebin_sjoerd(const CImg<> &raw, const int scale) {
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);
		
		// FT and fftshift
		CImgList<> f = raw.get_FFT();
		f[0].shift(raw.width() / 2, raw.height() / 2, 0, 0, 2);
		f[1].shift(raw.width() / 2, raw.height() / 2, 0, 0, 2);

		// Pad FT
		CImgList<> f_pad = CImgList<>(2, raw.width() * scale, raw.height() * scale, 1, 1, 0);
		int pad_x = raw.width() * (scale - 1);
		int pad_y = raw.height() * (scale - 1);
		int x0 = pad_x / 2;
		int y0 = pad_y / 2;
		f_pad[0].draw_image(x0, y0, 0, 0, f[0]);
		f_pad[1].draw_image(x0, y0, 0, 0, f[1]);

		// Inverse transform
		f_pad.FFT(true);
		cimg_forXY(scaled, x, y) {
			scaled(x, y) = round(sqrt(f_pad[0](x, y) * f_pad[0](x, y) + f_pad[1](x, y) * f_pad[1](x, y)));
		}

		CImg<> diff(scaled.width(), scaled.height(), 1, 1, 0);
		std::default_random_engine generator;
		cimg_forXY(diff, x, y) {
			std::poisson_distribution<int> pdist(scaled(x, y));
			diff(x, y) = pdist(generator) - scaled(x, y);
		}
		CImgList<> diff_ft = diff.get_FFT();
		diff_ft[0].shift(raw.width() / 2, raw.height() / 2, 0, 0, 2);
		diff_ft[1].shift(raw.width() / 2, raw.height() / 2, 0, 0, 2);
		
		diff_ft[0].draw_image(x0, y0, 0, 0, f[0]);
		diff_ft[1].draw_image(x0, y0, 0, 0, f[1]);

		diff_ft.FFT(true);
		cimg_forXY(scaled, x, y) {
			scaled(x, y) = round(sqrt(diff_ft[0](x, y) * diff_ft[0](x, y) + diff_ft[1](x, y) * diff_ft[1](x, y)));
		}

		return scaled;
	}

	
	CImg<> rebin_james(const CImg<> &raw, const int scale) {
		CImg<> scaled = fish::rebin_sjoerd(raw, scale);

		int excess_photons = round(scaled.sum() - raw.sum());
		printf("raw.sum() = %f, scaled.sum() = %f, excess_photons = %d\n", raw.sum(), scaled.sum(), excess_photons);
		std::default_random_engine generator;
		std::uniform_int_distribution<int> ddist(0, abs(excess_photons));
		if (excess_photons != 0) {
			printf("Altering photon number...\n");
			int inc = (excess_photons < 0) ? 1 : -1;
			for (int i = 0; i < abs(excess_photons); i++) {
				int ind = ddist(generator);
				scaled(ind) += inc;
			}
		}
		
		CImg<> errors = fish::rebin_down(scaled, scale) - raw;
		std::vector<int> deficiencies_x;
		std::vector<int> deficiencies_y;
		std::vector<int> deficiencies_n;
		cimg_forXY(errors, x, y) {
			if (errors(x, y) < 0) {
				deficiencies_x.push_back(x);
				deficiencies_y.push_back(y);
				deficiencies_n.push_back(-errors(x, y));
			}
		}
		cimg_forXY(errors, x, y) {
			if (errors(x, y) < 0) {
				continue;
			} else {
				int photon_num = errors(x, y);
				for (int i = 0; i < photon_num; i++) {
					// printf("%d pixels left to fill.\n", deficiencies_n.size());
					if (deficiencies_n.size() <= 0) {
						break;
					}
					std::uniform_int_distribution<int> ddist(0, deficiencies_n.size() - 1);
					int ind = ddist(generator);
					deficiencies_n[ind] -= 1;

					int nx = deficiencies_x[ind];
					int ny = deficiencies_y[ind];
					
					std::uniform_int_distribution<int> udist(0, scale - 1);
					
					int del_x = scale * x + udist(generator);
					int del_y = scale * y + udist(generator);
					int add_x = scale * nx + udist(generator);
					int add_y = scale * ny + udist(generator);
					
					// printf("Moving photon from (%d, %d) to (%d, %d).\n", del_x, del_y, add_x, add_y);

					scaled(del_x, del_y) -= 1;
					scaled(add_x, add_y) += 1;
					// printf("Photon reassignment complete.\n");
					// fflush(stdout);

					if (deficiencies_n[ind] == 0) {
						deficiencies_n.erase(deficiencies_n.begin() + ind);
						deficiencies_x.erase(deficiencies_x.begin() + ind);
						deficiencies_y.erase(deficiencies_y.begin() + ind);
					}
				}
			}
		}

		return scaled;
	}


	CImg<> rebin(const CImg<> &raw, const int scale, const char* direction) {
		CImg<> rebinned;

		int start_time = cimg::time();
		printf("\nRebinning image");
		if (!strcmp(direction, "down")) {
			printf(" down by %d...", scale);
			rebinned = rebin_down(raw, scale);
		} else if (!strcmp(direction, "up")) {
			printf(" up by %d...", scale);
			rebinned = rebin_up(raw, scale);
		} else if (!strcmp(direction, "up_nn")) {
			printf(" up_nn by %d...", scale);
			rebinned = rebin_up_nn(raw, scale);
		} else if (!strcmp(direction, "condense")) {
			printf(" up, using condense method, by %d...", scale);
			rebinned = rebin_condense(raw, scale);
		} else if (!strcmp(direction, "condense2")) {
			printf(" up, using condense2 method, by %d...", scale);
			rebinned = rebin_condense2(raw, scale);
		} else if (!strcmp(direction, "fourier")) {
			printf(" up, using fourier method, by %d...", scale);
			rebinned = rebin_fourier(raw, scale);
		} else if (!strcmp(direction, "sjoerd")) {
			printf(" up, using sjoerd method, by %d...", scale);
			rebinned = rebin_sjoerd(raw, scale);
		} else if (!strcmp(direction, "james")) {
			printf(" up, using james method, by %d...", scale);
			rebinned = rebin_james(raw, scale);
		} else {
			printf(" with direction '%s' not supported.", direction);
			exit(1);
		}
		int rebinning_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", rebinning_time);

		return rebinned;
	}
}
