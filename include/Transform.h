#pragma once

#include <glm/glm.hpp>

namespace tomway {
	// https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
	struct transform
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;

		transform();
	};
}

