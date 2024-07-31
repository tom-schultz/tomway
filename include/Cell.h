#pragma once

#include <vector>
#include <Vertex.h>

namespace hagl {
	class Cell
	{
	public:
		Cell(float x, float y, bool alive);
		void get_vertices(std::vector<Vertex>& vertices) const;
		static uint32_t vertices_per_cell();
	private:
		static constexpr float CELL_RADIUS = 0.3f;
		static constexpr float CELL_BORDER = 0.05f;
		float _x, _y;
		bool _alive;

		const static std::vector<Vertex> base_verts;
	};
}