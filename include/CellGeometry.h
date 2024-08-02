#pragma once
#include "CellContainer.h"

namespace tomway
{
    class CellGeometry
    {
    public:
        CellGeometry(CellContainer const& cells);
        std::vector<Vertex> const& get_vertices();
        size_t get_vertex_count() const;
    private:
        static std::vector<Vertex> const _base_verts;
        static float constexpr CELL_RADIUS = 0.5f;
        static float constexpr CELL_BORDER = 0.1f;
        static float constexpr CELL_POS_OFFSET = 2 * CELL_RADIUS + CELL_BORDER;
        CellContainer const& _cells;
		std::vector<Vertex> _vertices;
    };
}
