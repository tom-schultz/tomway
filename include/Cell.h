#pragma once

#include <vector>
#include <Vertex.h>

namespace hagl {
	class Cell
	{
	public:
		Cell(float x, float y, bool alive);
		void getVertices(std::vector<Vertex>& vertices);
		static uint32_t verticesPerCell();
	private:
		float _x, _y;
		bool _alive;

		const static std::vector<hagl::Vertex> baseVerts;
	};
}