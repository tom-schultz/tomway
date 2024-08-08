#include "Cell.h"


tomway::Cell::Cell(size_t x, size_t y, bool alive)
	: _x(x),
	_y(y),
	_alive(alive)
{
}

size_t tomway::Cell::get_x() const
{
	return _x;
}

size_t tomway::Cell::get_y() const
{
	return _y;
}

bool tomway::Cell::get_alive() const
{
	return _alive;
}

void tomway::Cell::set_alive(bool alive)
{
	_alive = alive;
}
