# Usage:  make        (build)      make run      (build + run)      make clean
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

biztrack: main.cpp $(wildcard structures/*.h modules/*.h models/*.h utils/*.h)
	$(CXX) $(CXXFLAGS) main.cpp -o biztrack

run: biztrack
	./biztrack

clean:
	rm -f biztrack
