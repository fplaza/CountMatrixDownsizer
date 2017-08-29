EXEC = count-matrix-downsizer

CXX = g++
CFLAGS = -Wall -Wextra -O3 -march=native -fopenmp
BOOST_LIBS = -lboost_program_options -lboost_filesystem -lboost_system
LDFLAGS = $(BOOST_LIBS) -fopenmp
DEFS = -D__APP_VERSION__="\"1.0.0\""

SRC = $(wildcard *.cc)
OBJECTS = $(patsubst %.cc, %.o, $(SRC))

.PHONY : clean

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cc
	$(CXX) $(CFLAGS) $(DEFS) -c $< -o $@

clean:
	rm -f $(EXEC) *.o
