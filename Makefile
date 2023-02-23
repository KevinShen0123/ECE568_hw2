TARGETS=proxy

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

test: test.cpp
	g++ -o -Werror -Wall $@ $< -l pthread

testClient: textClient.cpp
	g++ -o $@ $< -l pthread

proxy: proxy.cpp Cache.hpp SocketUtils.hpp 
	g++ -o $@ $< -l pthread