CXX = g++
COPTS = -g -Wall -std=c++17

laminate_main: laminate_main.o laminate.o input_parser.o ply.o
	$(CXX) $(COPTS) $^ -o $@ -I lib/eigen-3.3.7
	rm *.o

laminate_main.o: src/laminate_main.cc include/laminate.h include/input_parser.h include/ply.h
	$(CXX) $(COPTS) -c $< -o $@ -I lib/eigen-3.3.7

laminate.o: lib/laminate.cc include/laminate.h include/ply.h
	$(CXX) $(COPTS) -c $< -o $@ -I lib/eigen-3.3.7

input_parser.o: lib/input_parser.cc include/ply.h
	$(CXX) $(COPTS) -c $< -o $@ -I lib/eigen-3.3.7

ply.o: lib/ply.cc include/ply.h
	$(CXX) $(COPTS) -c $< -o $@ -I lib/eigen-3.3.7

clean:
	rm *.o