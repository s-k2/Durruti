DIRS    := src
SOURCES := $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJS    := $(patsubst %.cpp, %.o, $(SOURCES))
OBJS    := $(foreach o,$(OBJS),./obj/$(o))
DEPFILES:= $(patsubst %.o, %.P, $(OBJS))

INCLUDE = 
LIBS = 

# -pg  for profiling
GFLAGS	 = -O0 -g -std=c++17 -Wall
CFLAGS   = $(GFLAGS) -c -MMD $(INCLUDE)
LFLAGS   = $(GFLAGS) $(LIBS)
COMPILER = g++

#link the executable
durruti: $(OBJS)
	$(COMPILER) $(OBJS) $(LFLAGS) -o durruti

#generate dependency information and compile
obj/%.o : %.cpp
	@mkdir -p $(@D)
	$(COMPILER) $(CFLAGS) -o $@ -MF obj/$*.P $<

#remove all generated files
clean:
	rm -f analyze
	rm -rf obj

#include the dependency information
-include $(DEPFILES)
