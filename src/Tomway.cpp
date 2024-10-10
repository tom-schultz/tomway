// tomway.cpp : Defines the entry point for the application.

#include "tomway.h"
#include "engine.h"

size_t constexpr GRID_SIZE = 100;

int main(int argc, char* argv[])
{
	tomway::engine engine(GRID_SIZE);
	engine.run();
	exit(0);
}