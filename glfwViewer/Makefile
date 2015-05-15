CXX ?= g++
CXXFLAGS += -MP -MMD -Wall -g -O3
LDFLAGS += -Wall -lglfw -lGLEW

CXXSRC := glfwViewer.cpp

BIN := glfwViewer.exe
OBJ := $(CXXSRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

.PHONY: all
all: $(BIN)

-include $(DEP)

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	@$(CXX) -o $@ $^ $(LDFLAGS)

