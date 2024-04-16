#pragma once

#include <glm/glm.hpp>

namespace hagl {
	struct Transform
	{
		glm::vec3 translation;
		glm::vec4 rotation;
		glm::vec3 scale;
	};
}

