#include "Cell.h"

const std::vector<hagl::Vertex> hagl::Cell::base_verts = {
	{{-CELL_RADIUS, -CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
	{{CELL_RADIUS, -CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
	{{-CELL_RADIUS, CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
	{{CELL_RADIUS, -CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
	{{CELL_RADIUS, CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
	{{-CELL_RADIUS, CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
};

hagl::Cell::Cell(float x, float y, bool alive)
	: _x(x),
	_y(y),
	_alive(alive)
{
}

void hagl::Cell::get_vertices(std::vector<hagl::Vertex>& vertices) const
{
	if (_alive) {
		const std::vector<Vertex> verts(base_verts);

		constexpr auto pos_offset = 2 * CELL_RADIUS + CELL_BORDER;
		for (auto vertex : verts) {
			vertex.pos.x += _x * pos_offset;
			vertex.pos.y += _y * pos_offset;
			vertices.push_back(vertex);
		}
	}
}

uint32_t hagl::Cell::vertices_per_cell() {
	return static_cast<uint32_t>(base_verts.size());
}