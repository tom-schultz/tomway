#include "render/cell_geometry.h"

#include "Tracy.hpp"

std::vector<tomway::vertex> const tomway::cell_geometry::BASE_VERTS = {
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

tomway::cell_geometry::cell_geometry()
    : _cells(nullptr)
{
}

void tomway::cell_geometry::bind_cells(cell_container const* cells)
{
    ZoneScoped;
    _cells = cells;
	_vertices.resize(cells->size() * BASE_VERTS.size() + BACKGROUND_VERT_COUNT);
    _cells_dirty = true;
}

std::vector<vertex_chunk> tomway::cell_geometry::get_vertices(size_t max_chunk_alloc_size_bytes)
{
    ZoneScoped;
    
    if (_cells == nullptr or _cells->size() == 0)
    {
        _cells_dirty = false;
        return {};
    }

    if (not _cells_dirty)
    {
        return _chunks;
    }

    // Start with the max allowed per memory allocation
    size_t verts_per_chunk = max_chunk_alloc_size_bytes / sizeof(vertex);
    // Get rid of the remainder through integer division, then multiply up
    verts_per_chunk = verts_per_chunk / BASE_VERTS.size() * BASE_VERTS.size();
    size_t const max_verts_in_container = _cells->size() * BASE_VERTS.size();
    // If the maximum possible verts in our cell container is less than that, use that number instead
    verts_per_chunk = verts_per_chunk > max_verts_in_container ? max_verts_in_container : verts_per_chunk;
    
    size_t verts_acquired = BACKGROUND_VERT_COUNT;
    size_t verts_curr_chunk = BACKGROUND_VERT_COUNT;
    _chunks.clear();

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
        for (cell const& cell : *_cells)
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
                verts_curr_chunk += 1;
            }

            if (verts_curr_chunk >= verts_per_chunk)
            {
                _chunks.push_back({
                    _vertices.data() + verts_acquired - verts_per_chunk,
                    verts_curr_chunk,
                    verts_curr_chunk * sizeof(vertex),
                    verts_per_chunk * sizeof(vertex)});
                    
                verts_curr_chunk = 0;
            }
        }

        if (verts_curr_chunk)
        {
            _chunks.push_back({
                _vertices.data() + verts_acquired - verts_curr_chunk,
                verts_curr_chunk,
                verts_curr_chunk * sizeof(vertex),
                verts_per_chunk * sizeof(vertex)});
        }
        
        _cells_dirty = false;
    }
    
    return _chunks;
}

bool tomway::cell_geometry::is_dirty() const
{
    return _cells_dirty;
}