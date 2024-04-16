#include "Vertex.h"

vk::VertexInputBindingDescription hagl::Vertex::getBindingDescription() {
	vk::VertexInputBindingDescription desc(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
	return desc;
}

std::array<vk::VertexInputAttributeDescription, 2> hagl::Vertex::getAttributeDescriptions() {
	std::array<vk::VertexInputAttributeDescription, 2> ret{};

	ret[0] = { 0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, pos) }; // Location, binding, format, offset
	ret[1] = { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color) }; // Location, binding, format, offset

	return ret;
}