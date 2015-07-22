CXXFLAGS=-O3 -std=c++11 -Wall --pedantic -Wextra

HEADERS = \
	scene.h \
	imath.h \

SOURCES = \
	scene.cpp \
	imath.cpp \
	main.cpp \

LINKS=-lglut -lGL -lGLU

all: impulse

run: impulse
	./impulse

impulse: $(SOURCES) $(HEADERS)
	g++ $(CXXFLAGS) -o impulse $(SOURCES) $(LINKS)

clean:
	rm -f impulse
