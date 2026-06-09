CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra -Iinclude

all: lbm_channel

lbm_channel: src/main.cpp include/lbm.hpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o lbm_channel

run: lbm_channel
	./lbm_channel

clean:
	rm -f lbm_channel
	rm -f results/*.csv results/*.txt
