CXX = g++
CXXFLAGS = -std=c++11 -g -Wall

SOURCES = \
		main.cpp \


TARGET = CampusGuard

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run valgrind clean