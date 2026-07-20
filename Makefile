CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -pedantic -Iinclude
LDLIBS ?=

TARGET := lbm_channel
SRC := src/main.cpp
HDR := include/lbm.hpp

.PHONY: all run plots validate clean clean-all

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	@set -e; \
	if $(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS); then \
		:; \
	else \
		echo "Initial link failed; retrying with -lstdc++fs for older GCC toolchains."; \
		$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS) -lstdc++fs; \
	fi

run: $(TARGET)
	./$(TARGET)

plots:
	python3 scripts/plot_results.py

validate:
	python3 scripts/plot_results.py --max-relative-linf 0.05

clean:
	rm -f $(TARGET)
	rm -f results/*.csv results/*.txt

clean-all: clean
	rm -f results/*.png
