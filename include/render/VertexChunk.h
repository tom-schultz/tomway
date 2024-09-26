#pragma once
#include "Vertex.h"

struct VertexChunk
{
    tomway::Vertex const* vertices;
    size_t vertex_count;
    size_t data_size_bytes;
    size_t max_size_bytes;
};
