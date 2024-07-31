#pragma once

#include <vector>
#include <Vertex.h>

namespace hagl {
	class Cell
	{
	public:
		Cell(float x, float y, bool alive);
		void get_vertices(std::vector<Vertex>& vertices, size_t& vertex_count) const;
		static uint32_t vertices_per_cell();
	private:
		static const std::vector<Vertex> base_verts;
		static constexpr float CELL_RADIUS = 0.3f;
		static constexpr float CELL_BORDER = 0.05f;
		static constexpr float CELL_POS_OFFSET = 2 * CELL_RADIUS + CELL_BORDER;
		float _x, _y;
		bool _alive;
	};
}