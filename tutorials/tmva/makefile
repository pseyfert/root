######################################################################
# Project: TMVA - Toolkit for Multivariate Data Analysis             #
# Code   : Example usage for TMVA analyses                           #
######################################################################
BINS = TMVAClassification \
        TMVAClassificationCategory \
        TMVAClassificationApplication \
        TMVAClassificationCategoryApplication \
        TMVARegression \
        TMVARegressionApplication \
        TMVAMulticlass \
        TMVAMulticlassApplication \
        TMVAMultipleBackgroundExample \
			TMVAGAexample \
			TMVAGAexample2 \


CXX = $(shell root-config --cxx)

# later arguments override earlier ones. If root has been build with -std=c++1y then
# the examples should still see c++11 in compilation
CXXFLAGS = $(shell root-config --cflags) -O0 -ggdb -std=c++11


LD = $(shell root-config --ld)
LDFLAGS = $(shell root-config --ldflags) -g
LDLIBS = $(shell root-config --libs) -lMLP -lMinuit -lTreePlayer -lTMVA -lTMVAGui -lXMLIO  -lMLP -lm

INCS = $(shell root-config --incdir)


default : $(BINS)

$(BINS): % : %.C
	@echo -n "Building $@ ... "
	$(CXX) $(CXXFLAGS) $(addprefix -I, $(INCS)) $(LDFLAGS) $^ $(LDLIBS) -o $@
	@echo "Done"


clean:
	rm -f *.o	$(BINS)

