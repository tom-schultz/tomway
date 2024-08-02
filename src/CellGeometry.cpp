#include "CellGeometry.h"

std::vector<tomway::Vertex> const tomway::CellGeometry::_base_verts = {
    {{-CELL_RADIUS, -CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
    {{CELL_RADIUS, -CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
    {{-CELL_RADIUS, CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
    {{CELL_RADIUS, -CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
    {{CELL_RADIUS, CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
    {{-CELL_RADIUS, CELL_RADIUS}, {1.0f, 0.0f, 0.0f}},
};

tomway::CellGeometry::CellGeometry(CellContainer const& cells)
    : _cells(cells)
{
	_vertices.reserve(cells.size() * _base_verts.size());
}

std::vector<tomway::Vertex> const& tomway::CellGeometry::get_vertices()
{
    _vertices.resize(_vertices.capacity());
    size_t verts_acquired = 0;

    for (auto const cell : _cells)
    {
        if (not cell._alive) continue;
        
        for (auto const vertex : _base_verts)
        {
            auto const adjusted_cell_pos_x = (static_cast<float>(cell._x) - _cells.grid_size() / 2.0f) * CELL_POS_OFFSET;
            auto const adjusted_cell_pos_y = (static_cast<float>(cell._y) - _cells.grid_size() / 2.0f) * CELL_POS_OFFSET;
            _vertices[verts_acquired].pos.x = vertex.pos.x + adjusted_cell_pos_x;
            _vertices[verts_acquired].pos.y = vertex.pos.y + adjusted_cell_pos_y;
            _vertices[verts_acquired].color = vertex.color;
            verts_acquired += 1;
        }
    }

    _vertices.resize(verts_acquired);
    return _vertices;
}

size_t tomway::CellGeometry::get_vertex_count() const
{
    return _vertices.capacity();
}
