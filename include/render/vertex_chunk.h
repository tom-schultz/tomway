#pragma once
#include "vertex.h"

struct vertex_chunk
{
    tomway::vertex const* vertices;
    size_t vertex_count;
    size_t data_size_bytes;
    size_t max_size_bytes;
};
