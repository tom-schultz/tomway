#pragma once

#include <glm/glm.hpp>

namespace hagl {
	// https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
	struct Transform
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
	};
}

