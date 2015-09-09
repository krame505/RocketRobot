UNAME = $(shell uname)
#SHELL=/bin/bash -l

# Check g++ version
GCC_VER_GTE48 := $(shell echo `g++ -dumpversion | cut -f1-2 -d.` \>= 4.8 | sed -e 's/\./*100+/g' | bc )
ifneq ($(GCC_VER_GTE48),1)
$(error "g++ version 4.8 or newer is required")
endif

############################## Settings

EXECUTABLE      = ../bin/gorobot
TEST_EXECUTABLE = ../bin/testrobot

CPPC          = g++
CC            = gcc
CPPFLAGS      = -Wall -g -O3 -std=c++0x
CFLAGS 	      = -Wall -g -O3
GLUI 	      = glui
CONFIGURATION = configuration

#Every class to be included
CPPFILES += BaseGfxApp Simulation OptimizeSimulation
CPPFILES += PhysicalObject
CPPFILES += Robot Target Obstacle LightSource
CPPFILES += SimpleRobot ComplexRobot NeuralNetworkRobot NeuralNetwork
CPPFILES += Environment util
CPPFILES += Sensor
CPPFILES += Color artist
CPPFILES += main

#all the source files
SOURCES = $(addprefix ../src/,  $(CPPFILES:=.cpp))

#all the .o files
OBJECTS = $(addprefix ../bin/,  $(CPPFILES:=.o))

#Libraries to include
LINK_LIBS += -L../lib/$(GLUI)/lib/ -L../lib/$(CONFIGURATION)/lib/ 
LINK_LIBS += -lglui -lconfiguration
LINK_LIBS += -lpthread -lglut -lGL -lGLU
LINK_LIBS += -lboost_regex -lrt

############################## Rules

# This is the first rule in the file.
# It gets run when you just type "make"
default: setup $(SOURCES) $(EXECUTABLE)

all: setup $(SOURCES) $(EXECUTABLE) documentation

setup: ../bin

# Make sure bin/ exists
../bin:
	mkdir -p ../bin

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(TEST_EXECUTABLE) ../bin/test.cpp .doc *~

full-clean: clean
	rm -rf .glui .configuration
	$(MAKE) -C ../lib/$(GLUI) clean
	$(MAKE) -C ../doc clean

# used to specify which targets don't produce a
# file with the same name as the target.
# read about phony targets -> http://goo.gl/B6Ylvl
.PHONY: all setup clean full-clean tests gorobot testrobot default documentation $(GLUI)

############################## Compiling Robot
gorobot: $(EXECUTABLE)

$(EXECUTABLE): .glui .configuration $(OBJECTS) 
	$(CPPC) $(OBJECTS) $(LINK_LIBS) -o $@

## makefile note
##  in the rule "target: file1 file2"
##	$@ means everything to the left of : which is "target"
##	$< means the first file to the right of : which is "file1"

# places to look for included files
INCLUDE = -I../lib/$(GLUI)/include -I../lib/$(CONFIGURATION)/include

# compile all objects
../bin/%.o: ../src/%.cpp ../src/%.h
	$(CPPC) $< $(CPPFLAGS) $(INCLUDE) -c -o $@

# backup rule for objects without a header file (like main.o)
../bin/%.o: ../src/%.cpp
	$(CPPC) $< $(CPPFLAGS) $(INCLUDE) -c -o $@

# rule for compiling c files with headers
../bin/%.o: ../src/%.c ../src/%.h
	$(CC) $< $(CFLAGS) $(INCLUDE) -c -o $@

# rule for compiling c files without headers
../bin/%.o: ../src/%.c
	$(CC) $< $(CFLAGS) $(INCLUDE) -c -o $@

# rule for compiling generated c files
../bin/%.o: ../bin/%.c
	$(CC) $< $(CFLAGS) $(INCLUDE) -c -o $@

.glui: $(GLUI)
	touch .glui

.configuration: $(CONFIGURATION)
	touch .configuration

# this goes to ../lib/glui and does "make all"
$(GLUI):
	$(MAKE) -C ../lib/$(GLUI) all

# this goes to ../lib/configuration and does "make all"
$(CONFIGURATION):
	$(MAKE) -C ../lib/$(CONFIGURATION) all

############################## Doxygen
documentation: .doc
# src/.doc prevents the documentation from being regenerated
# when making all. delete .doc to regenerate doxygen

.doc: $(SOURCES) $(HEADERS) mainpage.h
	$(MAKE) -C ../doc all
	touch .doc
