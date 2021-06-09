#include "CImg.h"
#include "tinytiffwriter.h"

using namespace cimg_library;

namespace fish {
    void save_tiff(CImg<> &img, const char* filename, float pitch_xy, float spacing_z) {
        int start_time = cimg::time();
        TinyTIFFFile* tiff = TinyTIFFWriter_open(filename, 32, img.width(), img.height());
        if (tiff) {
            for (int slice = 0; slice < img.depth(); slice++) {
                float* data = img.data(0, 0, slice);
                TinyTIFFWriter_writeImageIJ(tiff, data, pitch_xy, spacing_z);
            }
            TinyTIFFWriter_close(tiff);
        }
        int out_time = cimg::time() - start_time;
        printf("Save time:     %d ms\n", out_time);
        printf("\n");
    }

    CImg<> load_tiff(const char* filename) {
        int start_time = cimg::time();
        CImg<> img(filename);
        int in_time = cimg::time() - start_time;
        printf("\nLoad time:     %d ms\n", in_time);
        printf("Dimensions:    %d x %d x %d\n", img.width(), img.height(), img.depth());

        return img;
    }
}