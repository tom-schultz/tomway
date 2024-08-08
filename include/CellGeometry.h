#pragma once
#include "CellContainer.h"

namespace tomway
{
    class CellGeometry
    {
    public:
        CellGeometry(CellContainer const* cells);
        void bind_cells(CellContainer const* cells);
        std::vector<Vertex> const& get_vertices();
        size_t get_vertex_count() const;
    private:
        static float constexpr BACKGROUND_VERT_COLOR = 0.025f;
        static unsigned int constexpr BACKGROUND_VERT_COUNT = 6;
        static std::vector<Vertex> const BASE_VERTS;
        static float constexpr CELL_BORDER = 0.1f;
        static float constexpr CELL_HEIGHT = 0.25f;
        static float constexpr CELL_WIDTH = 0.5f;
        static float constexpr CELL_POS_OFFSET = CELL_WIDTH + CELL_BORDER;
        static glm::vec3 constexpr COLOR_RED = {1.0f, 0.0f, 0.0f}; 
        CellContainer const* _cells;
		std::vector<Vertex> _vertices;
    };
}
