CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = -lrt

TARGETS = receiver normal_sender priority_sender

.PHONY: all clean

all: $(TARGETS)

receiver: receiver.cpp common.h
	$(CXX) $(CXXFLAGS) receiver.cpp -o receiver $(LDFLAGS)

normal_sender: normal_sender.cpp common.h
	$(CXX) $(CXXFLAGS) normal_sender.cpp -o normal_sender $(LDFLAGS)

priority_sender: priority_sender.cpp common.h
	$(CXX) $(CXXFLAGS) priority_sender.cpp -o priority_sender $(LDFLAGS)

clean:
	rm -f $(TARGETS)
