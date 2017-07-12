CXX ?= g++
CXXFLAGS += -MP -MMD -Wall -g -O3 $(shell freetype-config --cflags)
LDFLAGS += -Wall
LIBS := -lglfw -lGLEW -lGL ../libHexPlanet.a -lftgl

CXXSRC := glfwViewer.cpp camera.cpp controls.cpp shader.cpp hud.cpp

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

.PHONY: clean
clean:
	@rm -f $(OBJ) $(BIN)

