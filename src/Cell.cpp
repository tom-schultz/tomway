#include "Cell.h"

const std::vector<hagl::Vertex> hagl::Cell::baseVerts = {
	{{-0.45f, -0.45f}, {1.0f, 0.0f, 0.0f}},
	{{0.45f, -0.45f}, {1.0f, 0.0f, 0.0f}},
	{{-0.45f, 0.45f}, {1.0f, 0.0f, 0.0f}},
	{{0.45f, -0.45f}, {1.0f, 0.0f, 0.0f}},
	{{0.45f, 0.45f}, {1.0f, 0.0f, 0.0f}},
	{{-0.45f, 0.45f}, {1.0f, 0.0f, 0.0f}},
};

hagl::Cell::Cell(float x, float y, bool alive)
	: _x(x),
	_y(y),
	_alive(alive)
{
}

void hagl::Cell::getVertices(std::vector<hagl::Vertex>& vertices) {
	if (_alive) {
		std::vector<Vertex> verts(baseVerts);

		for (auto vertex : verts) {
			vertex.pos.x += _x;
			vertex.pos.y += _y;
			vertices.push_back(vertex);
		}
	}
}

uint32_t hagl::Cell::verticesPerCell() {
	return (uint32_t)baseVerts.size();
}