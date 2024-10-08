#include "simulation/cell.h"


tomway::cell::cell(size_t x, size_t y, bool alive)
	: _x(x),
	_y(y),
	_alive(alive)
{
}

size_t tomway::cell::get_x() const
{
	return _x;
}

size_t tomway::cell::get_y() const
{
	return _y;
}

bool tomway::cell::get_alive() const
{
	return _alive;
}

void tomway::cell::serialize(rapidjson::Document& doc, rapidjson::Value& cell_array) const
{
	if (not _alive) return;

	rapidjson::Value cell_value;
	cell_value.SetArray();
	cell_value.PushBack(_x, doc.GetAllocator());
	cell_value.PushBack(_y, doc.GetAllocator());
	cell_array.PushBack(cell_value, doc.GetAllocator());
}

void tomway::cell::set_alive(bool alive)
{
	_alive = alive;
}
