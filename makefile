TARGET = slam

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

CXX = g++
LIBS = -lraylib -lm
INCLUDE_DIRS =
LIB_DIRS =

DEBUG ?= 1

ifeq ($(DEBUG), 0)
    CXXFLAGS = -Wall -Wextra -std=c++17 -O3
else
    CXXFLAGS = -Wall -Wextra -std=c++17 -g
endif


all: debug

debug: CXXFLAGS += -g
debug: $(TARGET)

release: CXXFLAGS += -O3
release: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all debug release clean
