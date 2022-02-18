.PHONY: clean
basic: main.cpp soa.hpp
	$(CXX) -std=c++20 -O3 -march=native -g -o basic main.cpp
clean:
	rm basic