# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17

# Targets
TARGETS = writer reader

all: $(TARGETS)

writer: writer.cpp
	$(CXX) $(CXXFLAGS) writer.cpp -o writer

reader: reader.cpp
	$(CXX) $(CXXFLAGS) reader.cpp -o reader

clean:
	rm -f $(TARGETS)