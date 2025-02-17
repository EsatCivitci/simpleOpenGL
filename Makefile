CXX = g++
CXXFLAGS = -I./sb7code/include -std=c++17
LIBS = -lGL -lglfw -lGLEW

main: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o main $(LIBS)

clean:
	rm -f main
