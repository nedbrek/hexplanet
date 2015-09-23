CXX ?= g++
CXXFLAGS += -MP -MMD -Wall -g -O3
LDFLAGS += -Wall
LIBS := -lglfw -lGLEW -lGL ../libHexPlanet.a

CXXSRC := glfwViewer.cpp shader.cpp

BIN := glfwViewer.exe
OBJ := $(CXXSRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

.PHONY: all
all: $(BIN)

-include $(DEP)

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	@$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

