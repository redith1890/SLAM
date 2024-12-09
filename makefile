TARGET = slam

SOURCES = main.cpp array.cpp

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

LIBS = -lraylib -lm
INCLUDE_DIRS =
LIB_DIRS =

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
