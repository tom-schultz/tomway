#pragma once

#include "CellContainer.h"

namespace tomway
{
    class SimulationSystem
    {
    public:
        SimulationSystem(int grid_radius);
        size_t get_cell_count() const;
        CellContainer const& get_cells() const;
        void step_simulation();
    private:
        CellContainer _cells;
    };
}