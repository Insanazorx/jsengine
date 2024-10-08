CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++11 -g

SRC_FILES = *.cpp
TARGET = app

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC_FILES)

clean:
	rm -f $(TARGET)

run:
	./$(TARGET)

debug:
	lldb ./$(TARGET)