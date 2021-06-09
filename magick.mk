# Generic makefile for exe and libs
# author jerome boulanger jerome.boulanger@curie.fr
# assume that the exe depend on the libs
# For excecutable:
#   EXE = main etc
#   main_SRCS = main.cpp
#   main_LIBS = pthread X11 tiff m
# For libraries:
#   LIB = liba.a libb.a
#   liba.a_SRCS = file1.cpp file2.cpp
#   libb.a_SRCS = file3.cpp file4.cpp
# include magick.mk

.PHONY: all

all: debug

release: CXXFLAGS += -O3 -ffast-math -Dcimg_verbosity=0
release: $(LIB) $(EXE)

debug: CXXFLAGS += -DDEBUG -g -Dcimg_verbosity=3 -ggdb
debug: $(LIB) $(EXE)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.depends: %.cpp
	$(CXX) -M $(CXXFLAGS) $< > $@

# Template for the compiling programs
define EXE_template
$(strip $1): $(strip $1)_OBJS = $($(strip $1)_SRCS:.cpp=.o)
$(strip $1): $($(strip $1)_SRCS:.cpp=.o) $(LIB)
-include $($(strip $1)_SRCS:.cpp=.depends)
endef

# For each program expand the template
$(foreach prog,$(EXE),$(eval $(call EXE_template,$(prog))))

# Template for the compiling libraries
define LIB_template
$(strip $1): $(strip $1)_OBJS = $($(strip $1)_SRCS:.cpp=.o)
$(strip $1): $($(strip $1)_SRCS:.cpp=.o)
-include $($(strip $1)_SRCS:.cpp=.depends)
endef

# For each library expand the template
$(foreach prog,$(LIB),$(eval $(call LIB_template,$(prog))))

# Define targets
$(EXE):
	$(CXX) $(CXXFLAGS) $($@_OBJS) $($@_LIBS:%=-l%) -o $@

$(LIB):
	$(AR) rcs -o $@ $^

clean:
	rm -f $(LIB) $(EXE) *.o *~ *.depends
