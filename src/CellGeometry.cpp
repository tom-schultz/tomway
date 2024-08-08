#include "CellGeometry.h"

std::vector<tomway::Vertex> const tomway::CellGeometry::_base_verts = {
    {{0, 0}, {1.0f, 0.0f, 0.0f}},
    {{CELL_WIDTH, 0}, {1.0f, 0.0f, 0.0f}},
    {{0, CELL_WIDTH}, {1.0f, 0.0f, 0.0f}},
    {{CELL_WIDTH, 0}, {1.0f, 0.0f, 0.0f}},
    {{CELL_WIDTH, CELL_WIDTH}, {1.0f, 0.0f, 0.0f}},
    {{0, CELL_WIDTH}, {1.0f, 0.0f, 0.0f}},
};

tomway::CellGeometry::CellGeometry(CellContainer const* cells)
    : _cells(cells)
{
	_vertices.reserve(cells->size() * _base_verts.size() + BACKGROUND_VERT_COUNT);
}

void tomway::CellGeometry::bind_cells(CellContainer const* cells)
{
    _cells = cells;
}

std::vector<tomway::Vertex> const& tomway::CellGeometry::get_vertices()
{
    _vertices.resize(_vertices.capacity());
    size_t verts_acquired = BACKGROUND_VERT_COUNT;
    
    _vertices[0].pos.x = -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[0].pos.y = -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[0].color = {BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR};
    
    _vertices[1].pos.x =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[1].pos.y =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[1].color = {BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR};
    
    _vertices[2].pos.x =  -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[2].pos.y =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[2].color = {BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR};

    _vertices[3].pos.x = -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[3].pos.y = -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[3].color = {BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR};
    
    _vertices[4].pos.x =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[4].pos.y =  -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[4].color = {BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR};
    
    _vertices[5].pos.x =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[5].pos.y =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[5].color = {BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR, BACKGROUND_VERT_COLOR};

    for (Cell const& cell : *_cells)
    {
        if (not cell._alive) continue;
        
        for (auto const vertex : _base_verts)
        {
            auto const adjusted_cell_pos_x = (static_cast<float>(cell._x) - _cells->grid_size() / 2.0f) * CELL_POS_OFFSET;
            auto const adjusted_cell_pos_y = (static_cast<float>(cell._y) - _cells->grid_size() / 2.0f) * CELL_POS_OFFSET;
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
