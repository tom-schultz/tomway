#pragma once

#include "CellContainer.h"

namespace tomway
{
    class SimulationSystem
    {
    public:
        SimulationSystem(size_t grid_size);
        size_t get_cell_count() const;
        CellContainer const* current_cells() const;
        CellContainer const* step_simulation();
    private:
        inline size_t wrap(long long int val) const;
        size_t _grid_size;
        unsigned int _index = 0;
        CellContainer _cells[2];
    };
}