CXX ?= g++
CC ?= gcc
CXXFLAGS += -MP -MMD -Wall -g -O3
CFLAGS += -MP -MMD -Wall -g
LDFLAGS += -Wall -lGLEW -lglui -lGL

LIBSRC := src/hexplanet.cpp utils/neighbors.cpp src/load_texture.cpp
CXXSRC := $(LIBSRC) src/hexp_main.cpp src/planet_gui.cpp

CSRC := src/gamefontgl.c

BIN := hex_planet.exe
OBJ := $(CXXSRC:.cpp=.o) $(CSRC:.c=.o)
LIBOBJ := $(LIBSRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

.PHONY: all
all: $(BIN) libHexPlanet.a

-include $(DEP)

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

.c.o:
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	@$(CXX) -o $@ $^ $(LDFLAGS)

libHexPlanet.a: $(LIBOBJ)
	@ar r -s $@ $^

.PHONY: clean
clean:
	@rm -f $(OBJ) $(BIN)

