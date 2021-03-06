#include "fish.h"

int dim(int argc, char*argv[]) {
	cimg_help("\nDim image by factor");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float scale = cimg_option("-s", 1.0, "scaling factor");
	const bool display =   cimg_option("-display", false, "display dimmed image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::dim(img, scale);
	fish::save_tiff(img, file_out, 0, 0);

	if (display) {
		img.display("Dimmed image", false);
	}
	return 0;
}


int error(int argc, char*argv[]) {
	cimg_help("\nCalculate error between estimated error and ground truth");
	
	const char * file_est = cimg_option("-e", (char*) 0, "estimated image file");
	const char * file_truth = cimg_option("-t", (char*) 0, "ground truth image file");
	const char* method = cimg_option("-m", (char*) "rmse", "method\n");
	if (!file_est || !file_truth) {return 1;}

	CImg<> est = fish::load_tiff(file_est);
	CImg<> truth = fish::load_tiff(file_truth);
	double error = fish::error(est, truth, method);
	printf("Calculated error: %f\n", error);

	return 0;
}


int error_map(int argc, char*argv[]) {
	cimg_help("\nCalculate error between estimated error and ground truth");
	
	const char * file_est = cimg_option("-e", (char*) 0, "estimated image file");
	const char * file_truth = cimg_option("-t", (char*) 0, "ground truth image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const char* method = cimg_option("-m", (char*) "diff", "method\n");
	const bool display =   cimg_option("-display", false, "display error map\n");
	if (!file_est || !file_truth) {return 1;}

	CImg<> est = fish::load_tiff(file_est);
	CImg<> truth = fish::load_tiff(file_truth);
	CImg<> error = fish::error_map(est, truth, method);
	fish::save_tiff(error, file_out, 0, 0);

	if (display) {
		error.display("Error map", false);
	}
	return 0;
}


int intensify(int argc, char*argv[]) {
	cimg_help("\nIntensify image by factor");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float scale = cimg_option("-s", 1.0, "scaling factor");
	const bool display =   cimg_option("-display", false, "display intensified image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::intensify(img, scale);
	fish::save_tiff(img, file_out, 0, 0);

	if (display) {
		img.display("Intensified image", false);
	}
	return 0;
}


int poissonify(int argc, char* argv[]) {
	cimg_help("\nAdd Poisson noise to image");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float scale = cimg_option("-s", 1.0, "pre-scaling factor");
	const bool display =   cimg_option("-display", false, "display Poissonified image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::poissonify(img, scale);
	fish::save_tiff(img, file_out, 0, 0);

	if (display) {
		img.display("Poissonified image", false);
	}
	return 0;
}


int rebin(int argc, char* argv[]) {
	cimg_help("\nRebin image with new pixels");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const int scale = cimg_option("-s", 2, "scaling factor");
	const char* direction = cimg_option("-m", (char*) 0, "method [down, up_nn, up_fourier, up_fourier_poisson, up_thin_nn, up_thin_fourier, up_thin_fourier_poisson]");
	const bool display =   cimg_option("-display", false, "display rebinned image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::rebin(img, scale, direction);
	fish::save_tiff(img, file_out, 0, 0);

	if (display) {
		img.display("Rebinned image", false);
	}
	return 0;
}


int rebin_rl(int argc, char* argv[]) {
	cimg_help("\nRebin image with new pixels, using a modified Richardson-Lucy deconvolution for weights");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_psf = cimg_option("-p", (char*) 0, "PSF image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const int scale = cimg_option("-s", 2, "scaling factor");
	const int num_iters = cimg_option("-n", 10, "number of iterations");
	const bool display =   cimg_option("-display", false, "display rebinned image\n");
	if (!file_img || !file_out || !file_psf) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	CImg<> psf = fish::load_tiff(file_psf);
	img = fish::rebin_rl(img, scale, psf, num_iters);
	fish::save_tiff(img, file_out, 0, 0);

	if (display) {
		img.display("Rebinned image", false);
	}
	return 0;
}


int rotate(int argc, char*argv[]) {
	cimg_help("\nRotate image by angle");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float angle = cimg_option("-a", 45.0, "rotation angle (degrees)");
	const char* method = cimg_option("-m", "coord", "method [coord, nn]");
	const bool display =   cimg_option("-display", false, "display rotated image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::rotate(img, angle, method);
	fish::save_tiff(img, file_out, 0, 0);

	if (display) {
		img.display("Rotated image", false);
	}
	return 0;
}


int scale(int argc, char*argv[]) {
	cimg_help("\nScale image to new pixel pitch");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float pin = cimg_option("-pi", 0.0, "pixel pitch in");
	const float pout = cimg_option("-po", 0.0, "pixel pitch out");
	const bool display =   cimg_option("-display", false, "display rescaled image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::scale(img, pin, pout);
	fish::save_tiff(img, file_out, pout, 0);

	if (display) {
		img.display("Rescaled image", false);
	}
	return 0;
}


int split(int argc, char*argv[]) {
	cimg_help("\nSplit counts in image into two images");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out1 = cimg_option("-o1", (char*) 0, "output image file 1");
	const char * file_out2 = cimg_option("-o2", (char*) 0, "output image file 2");
	const float p1 = cimg_option("-p1", 0.0, "probability of assigning to image 1");
	if (!file_img || !file_out1 || !file_out2) {return 1;}

	printf("Doing stuff\n");
	CImg<> img = fish::load_tiff(file_img);
	CImgList<> split_images = fish::split(img, p1);
	fish::save_tiff(split_images[0], file_out1, 0, 0);
	fish::save_tiff(split_images[1], file_out2, 0, 0);

	return 0;
}


int translate(int argc, char*argv[]) {
	cimg_help("\nTranslate image with sub-pixel precision");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float shift_x = cimg_option("-x", 0.0, "shift in x");
	const float shift_y = cimg_option("-y", 0.0, "shift in y");
	const char* method = cimg_option("-m", "coord", "method [coord, binomial]");
	const bool display =   cimg_option("-display", false, "display translated image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::translate(img, shift_x, shift_y, method);
	fish::save_tiff(img, file_out, 0, 0);

	if (display) {
		img.display("Translated image", false);
	}
	return 0;
}


int show(int argc, char* argv[]) {
	cimg_help("\nDisplay image");
	CImg<> img = fish::load_tiff(argv[2]);
	img.display("Image", false);
	printf("\n");
	return 0;
}


int main(int argc, char* argv[]) {
	if (argc == 1) {
		printf("\nfish [command] [options] \n\n"
			   "Use one of the following commands:\n"
			   "  show\n"
			   "  dim\n"
			   "  affine\n"
			   "  intensify\n"
			   "  poissonify\n"
			   "  rotate\n"
			   "  scale\n"
			   "  translate\n"
			   "Use -h as an option to learn about each command.\n\n");
		return 0;
	}

	if (!strcmp(argv[1], "-h")) {
		main(1, argv);
	} else if (!strcmp(argv[1], "dim")) {
		return dim(argc, argv);
	} else if (!strcmp(argv[1], "error")) {
		return error(argc, argv);
	} else if (!strcmp(argv[1], "error_map")) {
		return error_map(argc, argv);
	} else if (!strcmp(argv[1], "intensify")) {
		return intensify(argc, argv);
	} else if (!strcmp(argv[1], "poissonify")) {
		return poissonify(argc, argv);
	} else if (!strcmp(argv[1], "rebin")) {
		return rebin(argc, argv);
	} else if (!strcmp(argv[1], "rebin_rl")) {
		return rebin_rl(argc, argv);
	} else if (!strcmp(argv[1], "rotate")) {
		return rotate(argc, argv);
	} else if (!strcmp(argv[1], "show")) {
		return show(argc, argv);
	} else if (!strcmp(argv[1], "scale")) {
		return scale(argc, argv);
	} else if (!strcmp(argv[1], "split")) {
		return split(argc, argv);
	} else if (!strcmp(argv[1], "translate")) {
		return translate(argc, argv);
	} else {
		printf("Command %s not implemented.\n", argv[1]);
	}

	return 0;
}