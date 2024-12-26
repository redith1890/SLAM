TARGET = slam

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

CXX = g++
LIBS = -lraylib -lm
INCLUDE_DIRS =
LIB_DIRS =

# Si no se define DEBUG, por defecto se activa el modo debug.
DEBUG ?= 1

ifeq ($(DEBUG), 0)
    CXXFLAGS = -Wall -Wextra -std=c++17 -O3  # Optimización para producción
else
    CXXFLAGS = -Wall -Wextra -std=c++17 -g  # Símbolos de depuración
endif

# Objetivo por defecto
all: debug

# Modo debug
debug: CXXFLAGS += -g
debug: $(TARGET)

# Modo release
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
