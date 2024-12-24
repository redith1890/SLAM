TARGET = slam

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g

LIBS = -lraylib -lm
INCLUDE_DIRS =
LIB_DIRS =

ifeq ($(DEBUG), 0)
    CXXFLAGS += -O3  # Optimización para producción (modo sin depuración)
else
    CXXFLAGS += -g  # Símbolos de depuración
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
