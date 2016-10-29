.DELETE_ON_ERROR:

######################################################################
# Project: TMVA - Toolkit for Multivariate Data Analysis             #
# Code   : Example usage for TMVA analyses                           #
###################################################################### 
BINS = TMVAClassification \
        TMVAClassificationCategory \
        TMVAClassificationApplication \
        TMVAClassificationApplication1 \
        TMVAClassificationApplication2 \
        TMVAClassificationApplication3 \
        TMVAClassificationApplication4 \
        TMVAClassificationApplication5 \
        TMVAClassificationApplication6 \
        TMVAClassificationCategoryApplication \
        TMVARegression \
        TMVARegressionApplication \
        TMVAMulticlass \
        TMVAMulticlassApplication \
        TMVAMultipleBackgroundExample \
			TMVAGAexample \
			TMVAGAexample2 \


CXX = g++
CCFLAGS = -std=c++11 


LD = g++
LDFLAGS = 

LIBS = $(shell root-config --libdir) 
INCS = $(shell root-config --incdir)  


default : $(BINS)

%.o : %.C 
	@echo -n "Compiling $@ ... "
	$(CXX) $(CCFLAGS) $<  $(addprefix -I, $(INCS)) -O2 -march=native -c -g -o $@

$(BINS): % : %.o 
	@echo -n "Linking $@ ... "
	$(CXX) $<  $(shell root-config --libs) -lMLP -lMinuit -lTreePlayer -lTMVA -lTMVAGui -lXMLIO  -lMLP -lm -O2 -march=native  -g -o $@

clean: 
	rm -f *.o	$(BINS)

callgrind%.out: TMVAClassificationApplication%
	valgrind --tool=callgrind -v --dump-instr=yes --trace-jump=yes --smc-check=all-non-file --callgrind-out-file=$@ ./$< > main$*.log > callgrind$*.log 2>&1
