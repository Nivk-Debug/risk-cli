CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -O2

risk: src/*.cpp src/*.h
	$(CXX) $(CXXFLAGS) -o risk src/*.cpp

clean:
	rm -f risk risk.exe *.bin

.PHONY: clean
