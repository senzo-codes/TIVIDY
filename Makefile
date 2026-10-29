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

# Automatically builds the target first, then runs it
run: $(TARGET)
	./TIVIDY

clean:
	rm -f $(OBJS) $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)