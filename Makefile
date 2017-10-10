
CPPFLAGS += -Iinclude

ipiptest : test/test.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $<

test/test.o: include/ipip.hpp test/test.cpp

all: test

clean:
	rm -rf test/test.o ipiptest
