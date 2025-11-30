# ----------------------------------------------------
#   Automatic Makefile for C++ Fundamentals Project
# ----------------------------------------------------

CXX       := g++
CXXFLAGS  := -std=c++20 -Wall -Wextra -pthread

# Source folders
FUND_SRC  := $(wildcard fundamentals/*.cpp)
IMPL_SRC  := $(wildcard implementations/*.cpp)

# Output folder
BIN_DIR   := bin

# Convert .cpp → bin/<filename>
FUND_BIN  := $(patsubst fundamentals/%.cpp, $(BIN_DIR)/%, $(FUND_SRC))
IMPL_BIN  := $(patsubst implementations/%.cpp, $(BIN_DIR)/%, $(IMPL_SRC))

ALL_BIN   := $(FUND_BIN) $(IMPL_BIN)

# Default target
all: $(BIN_DIR) $(ALL_BIN)
	@echo "Build complete."

# Ensure bin/ exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Rule to build binaries from fundamentals/
$(BIN_DIR)/%: fundamentals/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Rule to build binaries from implementations/
$(BIN_DIR)/%: implementations/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Clean build output
clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean
