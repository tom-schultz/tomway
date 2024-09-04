#include "CellGeometry.h"

std::vector<tomway::Vertex> const tomway::CellGeometry::BASE_VERTS = {
    // X, Y, CELL_WIDTH (Top)
    {{0.0f, 0.0f, CELL_HEIGHT}, {0.0f, 0.0f, 1.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, CELL_HEIGHT}, {0.0f, 0.0f, 1.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, CELL_HEIGHT}, {0.0f, 0.0f, 1.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, CELL_HEIGHT}, {0.0f, 0.0f, 1.0f}, COLOR_RED},
    {{CELL_WIDTH, CELL_WIDTH, CELL_HEIGHT}, {0.0f, 0.0f, 1.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, CELL_HEIGHT}, {0.0f, 0.0f, 1.0f}, COLOR_RED},
    
    // X, Y, 0 (Bottom)
    {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, 0.0f}, {0.0f, 0.0f, -1.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, 0.0f}, {0.0f, 0.0f, -1.0f}, COLOR_RED},
    {{CELL_WIDTH, CELL_WIDTH, 0.0f}, {0.0f, 0.0f, -1.0f}, COLOR_RED},
    
    // X, 0, Z (Back)
    {{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, COLOR_RED},
    {{0.0f, 0.0f, CELL_HEIGHT}, {0.0f, -1.0f, 0.0f}, COLOR_RED},
    {{0.0f, 0.0f, CELL_HEIGHT}, {0.0f, -1.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, CELL_HEIGHT}, {0.0f, -1.0f, 0.0f}, COLOR_RED},
    
    // X, CELL_WIDTH, Z (Front)
    {{CELL_WIDTH, CELL_WIDTH, CELL_HEIGHT}, {0.0f, 1.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, CELL_WIDTH, 0.0f}, {0.0f, 1.0f, 0.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, 0.0f}, {0.0f, 1.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, CELL_WIDTH, CELL_HEIGHT}, {0.0f, 1.0f, 0.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, 0.0f}, {0.0f, 1.0f, 0.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, CELL_HEIGHT}, {0.0f, 1.0f, 0.0f}, COLOR_RED},
    
    // 0, Y, Z (LEFT)
    {{0.0f, CELL_WIDTH, CELL_HEIGHT}, {-1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, 0.0f}, {-1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{0.0f, CELL_WIDTH, CELL_HEIGHT}, {-1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{0.0f, 0.0f, CELL_HEIGHT}, {-1.0f, 0.0f, 0.0f}, COLOR_RED},
    
    // CELL_WIDTH, Y, Z (RIGHT)
    {{CELL_WIDTH, CELL_WIDTH, CELL_HEIGHT}, {1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, CELL_WIDTH, 0.0f}, {1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, CELL_WIDTH, CELL_HEIGHT}, {1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, CELL_HEIGHT}, {1.0f, 0.0f, 0.0f}, COLOR_RED},
    {{CELL_WIDTH, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, COLOR_RED},
};

tomway::CellGeometry::CellGeometry(CellContainer const* cells)
    : _cells(cells)
{
	_vertices.reserve(cells->size() * BASE_VERTS.size() + BACKGROUND_VERT_COUNT);
}

void tomway::CellGeometry::bind_cells(CellContainer const* cells)
{
    _cells = cells;
}

std::vector<tomway::Vertex> const& tomway::CellGeometry::get_vertices()
{
    _vertices.resize(_vertices.capacity());
    size_t verts_acquired = BACKGROUND_VERT_COUNT;

    // UL
    _vertices[0].pos.x = -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[0].pos.y = -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[0].normal = {0.0f, 0.0f, 1.0f};
    _vertices[0].color = COLOR_LG;

    // LR
    _vertices[1].pos.x =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[1].pos.y =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[1].normal = {0.0f, 0.0f, 1.0f};
    _vertices[1].color = COLOR_DB;

    // LL
    _vertices[2].pos.x =  -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[2].pos.y =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[2].normal = {0.0f, 0.0f, 1.0f};
    _vertices[2].color = COLOR_DG;

    // UL
    _vertices[3].pos.x = -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[3].pos.y = -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[3].normal = {0.0f, 0.0f, 1.0f};
    _vertices[3].color = COLOR_LG;

    // UR
    _vertices[4].pos.x =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[4].pos.y =  -1.0f * _cells->grid_size() / 2.0f * CELL_POS_OFFSET;
    _vertices[4].normal = {0.0f, 0.0f, 1.0f};
    _vertices[4].color = COLOR_LB;

    // LR
    _vertices[5].pos.x =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[5].pos.y =  _cells->grid_size() / 2.0f * CELL_POS_OFFSET - CELL_BORDER;
    _vertices[5].normal = {0.0f, 0.0f, 1.0f};
    _vertices[5].color = COLOR_DB;

    for (Cell const& cell : *_cells)
    {
        if (not cell._alive) continue;
        
        auto const adjusted_cell_pos_x = (static_cast<float>(cell._x) - _cells->grid_size() / 2.0f) * CELL_POS_OFFSET;
        auto const adjusted_cell_pos_y = (static_cast<float>(cell._y) - _cells->grid_size() / 2.0f) * CELL_POS_OFFSET;
        
        for (auto const vertex : BASE_VERTS)
        {
            _vertices[verts_acquired].pos.x = vertex.pos.x + adjusted_cell_pos_x;
            _vertices[verts_acquired].pos.y = vertex.pos.y + adjusted_cell_pos_y;
            _vertices[verts_acquired].pos.z = vertex.pos.z;
            _vertices[verts_acquired].normal = vertex.normal;
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
