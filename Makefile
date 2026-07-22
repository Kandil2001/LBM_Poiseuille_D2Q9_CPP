CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -pedantic -Iinclude
LDLIBS ?=

TARGET := lbm_channel
SRC := src/main.cpp
HDR := include/lbm.hpp
TEST_TARGET := test_lbm
TEST_SRC := tests/test_lbm.cpp

.PHONY: all run plots test verify validate clean clean-all

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	@set -e; \
	if $(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS); then \
		:; \
	else \
		echo "Initial link failed; retrying with -lstdc++fs for older GCC toolchains."; \
		$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS) -lstdc++fs; \
	fi

$(TEST_TARGET): $(TEST_SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

plots:
	python3 scripts/plot_results.py

test: $(TEST_TARGET)
	./$(TEST_TARGET)

verify:
	python3 scripts/plot_results.py \
		--max-relative-linf 0.05 \
		--max-relative-mass-drift 1e-10 \
		--max-density-deviation 1e-8 \
		--max-transverse-velocity-ratio 1e-8 \
		--max-final-change 1e-8

# Backward-compatible alias retained for existing commands.
validate: verify

clean:
	rm -f $(TARGET) $(TEST_TARGET)
	rm -f results/*.csv results/*.txt

clean-all: clean
	rm -f results/*.png
