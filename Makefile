VERSION=0.1
CXXFLAGS=-g -W -Wall -O2 -std=c++0x
CXX=g++
COMMON_SOURCES= micter.cpp util.cpp
HEADERS = micter.hpp cmdline.h util.hpp
DIST_FILES=Makefile README.md $(COMMON_SOURCES) main.cpp micter_train.cpp

all: micter micter_train


micter: $(COMMON_SOURCES) $(HEADERS) main.cpp
	g++ $(COMMON_SOURCES) main.cpp -o micter $(CXXFLAGS)

micter_train: $(COMMON_SOURCES) $(HEADERS) micter_train.cpp
	g++ $(COMMON_SOURCES) micter_train.cpp -o micter-train $(CXXFLAGS)


test: svm_test
	./svm-test

a1a:
	wget "http://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary/a1a"
	wget "http://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary/a1a.t"

svm_test: $(COMMON_SOURCES) $(HEADERS) svm_test.cpp
	g++ $(COMMON_SOURCES) svm_test.cpp -o svm-test $(CXXFLAGS)

clean:
	rm -f *.o micter micter-train svm-test *~

dist:
	rm -rf micter-$(VERSION)
	mkdir micter-$(VERSION)
	cp -f $(DIST_FILES) micter-$(VERSION)
	tar cvzf micter-$(VERSION).tar.gz micter-$(VERSION)

