#pragma once

#include <string>

#include "simulation/cell_container.h"

namespace tomway
{
    class simulation_system
    {
    public:
        simulation_system();
        ~simulation_system() = default;
        simulation_system(simulation_system&) = delete;
        simulation_system(simulation_system&&) = delete;
        simulation_system& operator=(simulation_system const&) = delete;
        simulation_system& operator=(simulation_system const&&) = delete;
        
        void deserialize(std::string const& json);
        size_t get_cell_count() const;
        cell_container const* get_current_cells() const;
        void new_frame();
        std::string serialize() const;
        void start(size_t grid_size);
        void step_simulation();
    private:
        inline size_t wrap(long long int val) const;
        size_t _grid_size;
        unsigned int _index = 0;
        cell_container _cells[2];
    };
}