all:
	g++-10 main.cpp -Wall -Werror -O3 `sdl2-config --libs --cflags`
