#pragma once
#include "simulation/cell_container.h"
#include "vertex.h"
#include "render/vertex_chunk.h"

namespace tomway
{
    class cell_geometry
    {
    public:
        cell_geometry();
        ~cell_geometry() = default;
        cell_geometry(cell_geometry&) = delete;
        cell_geometry(cell_geometry&&) = delete;
        cell_geometry& operator=(cell_geometry const&) = delete;
        cell_geometry& operator=(cell_geometry const&&) = delete;
        
        void bind_cells(cell_container const* cells);
        std::vector<vertex_chunk> get_vertices(size_t max_chunk_alloc_size_bytes);
        bool is_dirty() const;
    private:
        static float constexpr BACKGROUND_VERT_COLOR = 0.025f;
        static unsigned int constexpr BACKGROUND_VERT_COUNT = 6;
        static std::vector<vertex> const BASE_VERTS;
        static float constexpr CELL_BORDER = 0.1f;
        static float constexpr CELL_HEIGHT = 0.25f;
        static float constexpr CELL_WIDTH = 0.5f;
        static float constexpr CELL_POS_OFFSET = CELL_WIDTH + CELL_BORDER;
        static glm::vec3 constexpr COLOR_RED = {1.0f, 0.0f, 0.0f}; 
        static glm::vec3 constexpr COLOR_LG = {0, 0.085f, 0}; 
        static glm::vec3 constexpr COLOR_DG = {0, 0.025f, 0}; 
        static glm::vec3 constexpr COLOR_LB = {0, 0, 0.085f}; 
        static glm::vec3 constexpr COLOR_DB = {0, 0, 0.025f}; 
        cell_container const* _cells;
        bool _cells_dirty = true;
        std::vector<vertex_chunk> _chunks;
        std::vector<vertex> _vertices;
    };
}
