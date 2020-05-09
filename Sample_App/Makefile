
.PHONY: all clean

all: ZT_Api


ZT_Api : ZT_Api.cpp
	g++ -std=c++11 ZT_Api.cpp utils.cpp -L=$(CURDIR) -lZT -lcrypto -Wl,--rpath=$(CURDIR) -o ztapi

