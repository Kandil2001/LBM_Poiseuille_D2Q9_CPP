CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -pedantic -Iinclude

TARGET := lbm_channel
SRC := src/main.cpp
HDR := include/lbm.hpp

.PHONY: all run plots clean clean-all

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

plots:
	python3 scripts/plot_results.py

clean:
	rm -f $(TARGET)
	rm -f results/*.csv results/*.txt

clean-all: clean
	rm -f results/*.png
