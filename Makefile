CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LIBS = -lsqlite3

integracion: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o integracion $(LIBS)

run: integracion
	./integracion

clean:
	rm -f integracion
