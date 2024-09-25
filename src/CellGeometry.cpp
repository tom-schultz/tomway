#include "CellGeometry.h"

#include "Tracy.hpp"

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
	_vertices.resize(cells->size() * BASE_VERTS.size() + BACKGROUND_VERT_COUNT);
}

void tomway::CellGeometry::bind_cells(CellContainer const* cells)
{
    ZoneScoped;
    _cells = cells;
	_vertices.resize(cells->size() * BASE_VERTS.size() + BACKGROUND_VERT_COUNT);
    _cells_dirty = true;
}

tomway::Vertex const* tomway::CellGeometry::get_vertices()
{
    ZoneScoped;
    
    if (_cells->size() == 0)
    {
        _vertex_count = 0;
        _cells_dirty = false;
        return _vertices.data();
    }

    if (not _cells_dirty)
    {
        return _vertices.data();
    }
    
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

    {
        ZoneScopedN("tomway::CellGeometry::get_vertices | Cell iteration");
        for (Cell const& cell : *_cells)
        {
            if (not cell._alive) continue;
            
            auto const adjusted_cell_pos_x = (static_cast<float>(cell._x) - _cells->grid_size() / 2.0f) * CELL_POS_OFFSET;
            auto const adjusted_cell_pos_y = (static_cast<float>(cell._y) - _cells->grid_size() / 2.0f) * CELL_POS_OFFSET;

            for (auto const& base_vert : BASE_VERTS)
            {
                auto& vert = _vertices[verts_acquired];
                vert.pos.x = base_vert.pos.x + adjusted_cell_pos_x;
                vert.pos.y = base_vert.pos.y + adjusted_cell_pos_y;
                vert.pos.z = base_vert.pos.z;
                vert.normal = base_vert.normal;
                vert.color = base_vert.color;
                verts_acquired += 1;
            }
        }
        
        _vertex_count = verts_acquired;
        _cells_dirty = false;
    }
    
    return _vertices.data();
}

size_t tomway::CellGeometry::get_vertex_count() const
{
    return _vertex_count;
}

bool tomway::CellGeometry::is_dirty() const
{
    return _cells_dirty;
}

size_t tomway::CellGeometry::max_vertex_count(size_t max_cells)
{
    return max_cells * BASE_VERTS.size();
}
