CXX ?= g++
CC ?= gcc
CXXFLAGS += -MP -MMD -Wall -g -O3
CFLAGS += -MP -MMD -Wall -g
LDFLAGS += -Wall -lGLEW -lglui -lGL

# build libHexPlanet
LIBSRC := src/hexplanet.cpp utils/neighbors.cpp
LIBOBJ := $(LIBSRC:.cpp=.o)
LIBDEP := $(LIBOBJ:.o=.d)
LIB := libHexPlanet.a

# build GL1 viewer
V1SRC  := viewer_gl1/hexp_main.cpp viewer_gl1/planet_gui.cpp viewer_gl1/load_texture.cpp
CSRC := viewer_gl1/gamefontgl.c
V1BIN := viewer_gl1/viewer.exe

V1OBJ := $(V1SRC:.cpp=.o) $(CSRC:.c=.o)
V1DEP := $(V1OBJ:.o=.d)

# all (for clean)
OBJ := $(V1OBJ) $(LIBOBJ)
BIN := $(V1BIN) $(LIB)

.PHONY: all
all: libHexPlanet.a

-include $(LIBDEP) $(V1DEP)

$(LIB): $(LIBOBJ)
$(V1BIN): $(V1OBJ) $(LIB)

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

.c.o:
	@$(CC) $(CFLAGS) -c -o $@ $<

%.exe:
	@$(CXX) -o $@ $^ $(LDFLAGS)

%.a:
	@ar r -s $@ $^

.PHONY: clean
clean:
	@rm -f $(OBJ) $(BIN)

