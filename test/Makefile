CXX ?= g++
CXXFLAGS += -MP -MMD -Wall -g -O3
LDFLAGS += -Wall
LIBS = -lGL -lGLEW

# make sure all sources get dependencies loaded
CXXSRC := ../src/hexplanet.cpp ../src/load_texture.cpp loadTest.cpp
OBJ := $(CXXSRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

.PHONY: all
all: loadTest.exe

LAST := 6
NUMBERS := $(shell seq 1 $(LAST))
TESTS := $(addprefix test, $(NUMBERS))

.PHONY: test $(TESTS)
test: $(TESTS)

$(TESTS): test%: loadTest.exe
	@./loadTest.exe -l $* > sphere$*.test.obj
	diff sphere$*.test.obj sphere$*.fixed.obj

-include $(DEP)

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

loadTest.exe: $(OBJ)
	@$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

