#include "SimulationSystem.h"

tomway::SimulationSystem::SimulationSystem(int const grid_radius)
    : _cells(grid_radius)
{
}

size_t tomway::SimulationSystem::get_cell_count() const
{
    return _cells.size();
}

tomway::CellContainer const& tomway::SimulationSystem::get_cells() const
{
    return _cells;
}

void tomway::SimulationSystem::step_simulation()
{
    for (auto cell : _cells)
    {
        int neighbors_alive = 0;

        for (int x = -1; x <= 1 && neighbors_alive < 4; x++)
        {
            for (int y = -1; y <= 1 && neighbors_alive < 4; y++)
            {
                if (x == 0 && y == 0) continue;
            }
        }
    }
}