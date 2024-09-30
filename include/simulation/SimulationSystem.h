#pragma once

#include <string>

#include "simulation/CellContainer.h"

namespace tomway
{
    class SimulationSystem
    {
    public:
        SimulationSystem();
        void deserialize(std::string const& json);
        size_t get_cell_count() const;
        CellContainer const* get_current_cells() const;
        void new_frame();
        std::string serialize() const;
        void start(size_t grid_size);
        void step_simulation();
    private:
        inline size_t wrap(long long int val) const;
        size_t _grid_size;
        unsigned int _index = 0;
        CellContainer _cells[2];
    };
}