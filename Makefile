CXX=       	g++
CXXFLAGS= 	-g -gdwarf-2 -std=gnu++11 -Wall -fPIC
LDFLAGS=	-pthread

all:    uthread

%.o:	%.cpp uthread.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

uthread:	uthread.o main.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) -f uthread uthread.o main.o

.PHONY: all clean

