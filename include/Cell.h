#pragma once

#include <vector>
#include <Vertex.h>

namespace tomway {
	class CellGeometry;

	class Cell
	{
	public:
		Cell(size_t x, size_t y, bool alive);
		size_t get_x() const;
		size_t get_y() const;
		bool get_alive() const;
		void set_alive(bool alive);
	private:
		size_t _x, _y;
		bool _alive;
        friend CellGeometry;
	};
}
