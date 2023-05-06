CXX = g++
CXXFLAGS = -g -Wall -O2
PROGNAME = chat-server
OBJMODULES = EventSelector.o FdHandler.o Server.o Session.o main.o
PORT = 3000

build: $(OBJMODULES)
	$(CXX) $(CXXFLAGS) $^ -o $(PROGNAME)

run-dev: build
	./$(PROGNAME) $(PORT)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o $(PROGNAME) *.dSYM deps.mk

deps.mk: $(wildcard *.cpp) Makefile
	$(CXX) -MM $(wildcard *.cpp) > $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

