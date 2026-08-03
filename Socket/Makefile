CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGETS = server client

.PHONY: all clean

all: $(TARGETS)

server: server.cpp common.h
	$(CXX) $(CXXFLAGS) server.cpp -o server

client: client.cpp common.h
	$(CXX) $(CXXFLAGS) client.cpp -o client

clean:
	rm -f $(TARGETS)
	rm -f /tmp/demo_unix_socket