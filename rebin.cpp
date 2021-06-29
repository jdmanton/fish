#include "CImg.h"
#include "fish.h"
#include <random>
#include <cassert>
#include <vector>
#include <iostream>

using namespace cimg_library;


CImgList<> psf2otf(const CImg<>& psf, const int width, const int height, const int depth) {
	int x_max_ind = 0, y_max_ind = 0, z_max_ind = 0;
	double v_max = 0, sum = 0;
	cimg_forXYZ(psf, x, y, z) {
		const double v = psf(x, y, z);
		sum += v;
		if (v > v_max) {
			v_max = v;
			x_max_ind = x;
			y_max_ind = y;
			z_max_ind = z;
		}
	}
	CImg<> h(width, height, depth, 1, 0);
	const int
		x0 = cimg::round((float) width / 2.0f - (float) x_max_ind),
		y0 = cimg::round((float) width / 2.0f - (float) y_max_ind),
		z0 = depth == 1 ? 0 : round((float) depth / 2.0f - (float) z_max_ind);
	h.draw_image(x0, y0, z0, psf);
	h = h / (float) sum;
	h.shift(width / 2, height / 2, depth / 2, 0, 2);
	return h.get_FFT();
}


namespace fish{
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


	CImg<> rebin_up_fourier(const CImg<> &raw, const int scale) {
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


	CImg<> rebin_up_fourier_poisson(const CImg<> &raw, const int scale) {
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

	
	CImg<> rebin_up_weighted(const CImg<> &raw, const CImg<> &weights, const int scale) {
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);

		std::default_random_engine generator;

		cimg_forXY(raw, x, y) {
			int num_photons = raw(x, y);
			std::vector<float> weight_vec(scale * scale);

			for (int i = 0; i < scale * scale; i++) {
				int xb = i % scale;
				int yb = i / scale;
				int xs = x * scale + xb;
				int ys = y * scale + yb;
				weight_vec[i] = weights(xs, ys);
			}
			
			std::discrete_distribution<int> ddist(weight_vec.begin(), weight_vec.end());
			
			for (int i = 0; i < num_photons; i++) {
				int ind = ddist(generator);
				int xb = ind % scale;
				int yb = ind / scale;
				int xs = x * scale + xb;
				int ys = y * scale + yb;
				scaled(xs, ys) += 1;
			}
		}

		return scaled;
	}

	
	CImg<> rebin_up_thin_nn(const CImg<> &raw, const int scale) {
		CImg<> weights = fish::rebin_up_nn(raw, scale).round();
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);
		
		return fish::rebin_up_weighted(raw, weights, scale);;    
	}
	
	
	CImg<> rebin_up_thin_fourier(const CImg<> &raw, const int scale) {
		CImg<> weights = fish::rebin_up_fourier(raw, scale).round();
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);
		
		return fish::rebin_up_weighted(raw, weights, scale);;    
	}


	CImg<> rebin_up_thin_fourier_poisson(const CImg<> &raw, const int scale) {
		CImg<> weights = fish::rebin_up_fourier_poisson(raw, scale).round();
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);

		return fish::rebin_up_weighted(raw, weights, scale);;    
	}


	CImg<> rebin_rl(const CImg<> &raw, const int scale, const CImg<> &psf, const int num_iters) {
		// Use a blurred RL deconvolution of the image to provide the weights for photon reassignment
		
		CImg<> scaled(raw.width() * scale, raw.height() * scale, 1, 1, 0);

		const float eps = 1e-2;

		CImgList<> H = psf2otf(psf, psf.width(), psf.height(), psf.depth());
		CImgList<> H_unbinned = psf2otf(fish::rebin_up_nn(psf, scale), scale * psf.width(), scale * psf.height(), scale * psf.depth());

		// CImg<> estimate = fish::rebin_up_nn(raw, scale).get_max(eps).blur(4.0f);
		CImg<> estimate(scaled.width(), scaled.height(), 1, 1, raw.mean());
		CImg<> estimate_binned;
		CImgList<> temp(2, scaled.width(), scaled.height(), scaled.depth(), 1, 0);
		CImgList<> temp_binned(2, raw.width(), raw.height(), raw.depth(), 1, 0);

		

		for (int i = 0; i < num_iters; i++) {
			// Bin down
			estimate_binned = fish::rebin_down(estimate, scale);
			
			// Convolve with PSF (Fourier-wise with H)
			temp_binned = estimate_binned.get_FFT();
			cimg_foroff(H[0], i) {
				const float a = temp_binned[0](i), b = temp_binned[1](i), c = H[0](i), d = H[1](i);
				temp_binned[0](i) = a*c - b*d;
				temp_binned[1](i) = a*d + b*c;
			}
			temp_binned.FFT(true);
			
			// Compute ratio
			cimg_foroff(temp_binned[0], i) {
				if (temp_binned[0](i) > 0)
					temp_binned[0](i) = std::max(eps, raw(i)) / temp_binned[0](i);
				temp_binned[1](i) = 0;
			}
			
			// Unbin
			temp[0] = fish::rebin_up_nn(temp_binned[0], scale);
			temp[1] = fish::rebin_up_nn(temp_binned[1], scale);
			
			// Convolve with transpose of PSF (Fourier-wise with H_unbinned)
			// (at the moment there is no transpose, just conjugation, as we 'know' that the PSF is real)
			temp.FFT();
			cimg_foroff(temp[0], i) {
				const float a = temp[0](i), b = temp[1](i), c = H_unbinned[0](i), d = -H_unbinned[1](i);
				temp[0](i) = a*c - b*d;
				temp[1](i) = a*d + b*c;
			}
			temp.FFT(true);
			
			// Update estimate via multiplication
			estimate.mul(temp[0]);
		}

		// Reblur
		temp = estimate.get_FFT();
		cimg_foroff(temp[0], i) {
			const float a = temp[0](i), b = temp[1](i), c = H_unbinned[0](i), d = H_unbinned[1](i);
			temp[0](i) = a*c - b*d;
			temp[1](i) = a*d + b*c;
		}
		temp.FFT(true);
		estimate = temp[0];

		return fish::rebin_up_weighted(raw, estimate, scale);
	}


	CImg<> rebin(const CImg<> &raw, const int scale, const char* method) {
		CImg<> rebinned;

		int start_time = cimg::time();
		printf("\nRebinning image");
		if (!strcmp(method, "down")) {
			printf(" down by %d...", scale);
			rebinned = rebin_down(raw, scale);
		} else if (!strcmp(method, "up_nn")) {
			printf(" up_nn by %d...", scale);
			rebinned = rebin_up_nn(raw, scale);
		} else if (!strcmp(method, "up_fourier")) {
			printf(" up, using fourier method, by %d...", scale);
			rebinned = rebin_up_fourier(raw, scale);
		} else if (!strcmp(method, "up_fourier_poisson")) {
			printf(" up, using fourier_poisson method, by %d...", scale);
			rebinned = rebin_up_fourier_poisson(raw, scale);
		} else if (!strcmp(method, "up_thin_nn")) {
			printf(" up, using up_thin_nn method, by %d...", scale);
			rebinned = rebin_up_thin_nn(raw, scale);
		} else if (!strcmp(method, "up_thin_fourier")) {
			printf(" up, using up_thin_fourier method, by %d...", scale);
			rebinned = rebin_up_thin_fourier(raw, scale);
		} else if (!strcmp(method, "up_thin_fourier_poisson")) {
			printf(" up, using up_thin_fourier_poisson method, by %d...", scale);
			rebinned = rebin_up_thin_fourier_poisson(raw, scale);
		} else {
			printf(" with method '%s' not supported.", method);
			exit(1);
		}
		int rebinning_time = cimg::time() - start_time;
		printf(" (completed in %d ms)\n", rebinning_time);

		return rebinned;
	}
}
