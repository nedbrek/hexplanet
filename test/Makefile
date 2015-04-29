CXX ?= g++
CXXFLAGS += -MP -MMD -Wall -g -O3
LDFLAGS += -Wall
LIBS = -lGL -lGLEW

# make sure all sources get dependencies loaded

# loadTest
LT_CXXSRC := ../src/hexplanet.cpp ../src/load_texture.cpp loadTest.cpp
LT_OBJ := $(LT_CXXSRC:.cpp=.o)

# writeData
WD_CXXSRC := ../src/hexplanet.cpp ../src/load_texture.cpp writeData.cpp
WD_OBJ := $(WD_CXXSRC:.cpp=.o)

OBJ := $(LT_OBJ) $(WD_OBJ)
DEP := $(OBJ:.o=.d)

.PHONY: all
all: loadTest.exe writeData.exe

LAST := 8
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

loadTest.exe: $(LT_OBJ)
	@$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

writeData.exe: $(WD_OBJ)
	@$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

