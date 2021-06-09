#include "fish.h"

int intensify(int argc, char*argv[]) {
	cimg_help("\nIntensify image by factor");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float scale = cimg_option("-s", 1.0, "scaling factor");
	const char* method = cimg_option("-m", "binom", "method [binom, coin]");
	const bool display =   cimg_option("-d", false, "display intensified image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::intensify(img, scale, method);
	fish::save_tiff(img, file_out, 0, 0);

	if (display) {
		img.display("Intensified image", false);
	}
	return 0;
}


int rotate(int argc, char*argv[]) {
	cimg_help("\nRotate image by angle");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float angle = cimg_option("-a", 45.0, "rotation angle (degrees)");
	const char* method = cimg_option("-m", "coord", "method [coord, nn]");
	const bool display =   cimg_option("-d", false, "display rotated image\n");
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
	const bool display =   cimg_option("-d", false, "display rescaled image\n");
	if (!file_img || !file_out) {return 1;}

	CImg<> img = fish::load_tiff(file_img);
	img = fish::scale(img, pin, pout);
	fish::save_tiff(img, file_out, pout, 0);

	if (display) {
		img.display("Rescaled image", false);
	}
	return 0;
}


int translate(int argc, char*argv[]) {
	cimg_help("\nTranslate image with sub-pixel precision");
	
	const char * file_img = cimg_option("-i", (char*) 0, "input image file");
	const char * file_out = cimg_option("-o", (char*) 0, "output image file");
	const float shift_x = cimg_option("-x", 0.0, "shift in x");
	const float shift_y = cimg_option("-y", 0.0, "shift in y");
	const char* method = cimg_option("-m", "coord", "method [coord, binomial]");
	const bool display =   cimg_option("-d", false, "display translated image\n");
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
			   "  affine\n"
			   "  intensify\n"
			   "  rotate\n"
			   "  scale\n"
			   "  translate\n"
			   "Use -h as an option to learn about each command.\n\n");
		return 0;
	}

	if (!strcmp(argv[1], "-h")) {
		main(1, argv);
	} else if (!strcmp(argv[1], "intensify")) {
		return intensify(argc, argv);
	} else if (!strcmp(argv[1], "rotate")) {
		return rotate(argc, argv);
	} else if (!strcmp(argv[1], "show")) {
		return show(argc, argv);
	} else if (!strcmp(argv[1], "scale")) {
		return scale(argc, argv);
	} else if (!strcmp(argv[1], "translate")) {
		return translate(argc, argv);
	} else {
		printf("Command %s not implemented.\n", argv[1]);
	}

	return 0;
}