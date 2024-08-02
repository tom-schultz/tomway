#pragma once

#include <vector>
#include <Vertex.h>

namespace tomway {
	class CellGeometry;

	class Cell
	{
	public:
		Cell(size_t x, size_t y, bool alive);
	private:
		size_t _x, _y;
		bool _alive;
        friend CellGeometry;
	};
}
