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

void hagl::Cell::get_vertices(std::vector<hagl::Vertex>& vertices, size_t& vertex_count) const
{
	if (!_alive) return;

	for (auto const vertex : base_verts) {
		vertices[vertex_count].pos.x = vertex.pos.x + _x * CELL_POS_OFFSET;
		vertices[vertex_count].pos.y = vertex.pos.y + _y * CELL_POS_OFFSET;
		vertices[vertex_count].color = vertex.color;
		vertex_count += 1;
	}
}

uint32_t hagl::Cell::vertices_per_cell() {
	return static_cast<uint32_t>(base_verts.size());
}