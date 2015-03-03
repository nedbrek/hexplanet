CXX ?= g++
CXXFLAGS += -MP -MMD -Wall -g
LDFLAGS += -Wall -lGLEW -lGL

CXXSRC := ../src/hexplanet.cpp ../src/load_texture.cpp loadTest.cpp
OBJ := $(CXXSRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

.PHONY: all
all: loadTest.exe

-include $(DEP)

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

loadTest.exe: $(OBJ)
	@$(CXX) -o $@ $^ $(LDFLAGS)

