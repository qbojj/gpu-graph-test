CXX := clang++
CC := clang
CFLAGS := -O2 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-private-field
CXXFLAGS := -std=c++20 $(CFLAGS)
LIBS := -lglfw -lGL
INCLUDES := -I./ -I./glad/include

SOURCES_CPP := $(wildcard *.cpp)
SOURCES_C := $(wildcard glad/src/*.c)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

gpu-graph-test: $(SOURCES_CPP:.cpp=.o) $(SOURCES_C:.c=.o)
	$(CXX) $(CXXFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f $(SOURCES_CPP:.cpp=.o) $(SOURCES_C:.c=.o)
	rm -f gpu-graph-test