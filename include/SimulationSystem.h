#pragma once

#include "CellContainer.h"

namespace tomway
{
    class SimulationSystem
    {
    public:
        SimulationSystem();
        size_t get_cell_count() const;
        CellContainer const* get_current_cells() const;
        void start(size_t const grid_size);
        void step_simulation();
    private:
        inline size_t wrap(long long int val) const;
        size_t _grid_size;
        unsigned int _index = 0;
        CellContainer _cells[2];
    };
}