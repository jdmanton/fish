#depends on libtiff-dev libfftw3-dev

# to cross compile for windows using MXE use:
# make CROSS=x86_64-w64-mingw32.static- lsfm.exe

CXX=$(CROSS)g++

PKG = fish
VER = 0.1

CXXFLAGS = -Dcimg_use_cpp11 -Dcimg_use_openmp -Dcimg_use_tiff -Dcimg_use_fftw3 -std=c++11 -fopenmp -pthread -L./

EXE = fish

fish_SRCS = fish.cpp
fish_LIBS = fish tiff jpeg lzma z fftw3_omp fftw3 m X11

LIB = libfish.a

libfish.a_SRCS = intensify.cpp io.cpp misc.cpp rotate.cpp scale.cpp translate.cpp tinytiffwriter.cpp
libfish.a_LIBS = fftw3_omp fftw3 m

include magick.mk
