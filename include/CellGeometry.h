#pragma once
#include "simulation/CellContainer.h"
#include "Vertex.h"

namespace tomway
{
    class CellGeometry
    {
    public:
        CellGeometry(CellContainer const* cells);
        void bind_cells(CellContainer const* cells);
        Vertex const* get_vertices();
        size_t get_vertex_count() const;
        bool is_dirty() const;
        static size_t max_vertex_count(size_t max_cells);
    private:
        static float constexpr BACKGROUND_VERT_COLOR = 0.025f;
        static unsigned int constexpr BACKGROUND_VERT_COUNT = 6;
        static std::vector<Vertex> const BASE_VERTS;
        static float constexpr CELL_BORDER = 0.1f;
        static float constexpr CELL_HEIGHT = 0.25f;
        static float constexpr CELL_WIDTH = 0.5f;
        static float constexpr CELL_POS_OFFSET = CELL_WIDTH + CELL_BORDER;
        static glm::vec3 constexpr COLOR_RED = {1.0f, 0.0f, 0.0f}; 
        static glm::vec3 constexpr COLOR_LG = {0, 0.085f, 0}; 
        static glm::vec3 constexpr COLOR_DG = {0, 0.025f, 0}; 
        static glm::vec3 constexpr COLOR_LB = {0, 0, 0.085f}; 
        static glm::vec3 constexpr COLOR_DB = {0, 0, 0.025f}; 
        CellContainer const* _cells;
        bool _cells_dirty = true;
        std::vector<Vertex> _vertices;
        size_t _vertex_count = 0;
    };
}
