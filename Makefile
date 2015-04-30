CXX ?= g++
CC ?= gcc
CXXFLAGS += -MP -MMD -Wall -g -O3
CFLAGS += -MP -MMD -Wall -g
LDFLAGS += -Wall -lGLEW -lglui -lGL

CXXSRC := src/hexplanet.cpp src/hexp_main.cpp src/load_texture.cpp \
src/planet_gui.cpp utils/neighbors.cpp

CSRC := src/gamefontgl.c

BIN := hex_planet.exe
OBJ := $(CXXSRC:.cpp=.o) $(CSRC:.c=.o)
DEP := $(OBJ:.o=.d)

.PHONY: all
all: $(BIN)

-include $(DEP)

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

.c.o:
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	@$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	@rm -f $(OBJ) $(BIN)

