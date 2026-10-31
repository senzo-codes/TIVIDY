CXX = g++
CXXFLAGS = -std=c++11 -g -Wall
TARGET = TIVIDY

# Automatically find all .cpp files (main.cpp, WorkflowEngine.cpp, Role.cpp, etc.)
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile any .cpp file into an .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


TEST_SRCS = $(wildcard tests/*.cpp)
LIB_SRCS  = $(filter-out main.cpp, $(SRCS))

test: $(TEST_SRCS) $(LIB_SRCS)
	$(CXX) -std=c++11 -g -I. -Itests $^ -o tests/run_tests
	./tests/run_tests
	
# Automatically builds the target first, then runs it
run: $(TARGET)
	./TIVIDY

clean:
	rm -f $(OBJS) $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)