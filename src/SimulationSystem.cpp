#include "SimulationSystem.h"

#include "HaglUtility.h"

tomway::SimulationSystem::SimulationSystem(size_t const grid_size)
    : _grid_size(grid_size),
    _cells{ {grid_size}, {grid_size}}
{
}

size_t tomway::SimulationSystem::get_cell_count() const
{
    return _cells[_index].size();
}

tomway::CellContainer const* tomway::SimulationSystem::current_cells() const
{
    return &_cells[_index];
}

tomway::CellContainer const* tomway::SimulationSystem::step_simulation()
{
    unsigned int const new_index = (_index + 1) % 2;
    
    for (Cell& cell : _cells[_index])
    {            
        size_t const x = cell.get_x();
        size_t const y = cell.get_y();
        
        size_t const xp1 = wrap(x + 1);
        size_t const xm1 = wrap(x - 1);
        size_t const yp1 = wrap(y + 1);
        size_t const ym1 = wrap(y - 1);

        int const neighbors_alive = _cells[_index].get_alive(xm1, y) // Left
             + _cells[_index].get_alive(xm1, ym1) // Upper Left
             + _cells[_index].get_alive(x, ym1) // Up
             + _cells[_index].get_alive(xp1, ym1) // Upper Right
             + _cells[_index].get_alive(xp1, y) // Right
             + _cells[_index].get_alive(xp1, yp1) // Lower Right
             + _cells[_index].get_alive(x, yp1) // Down
             + _cells[_index].get_alive(xm1, yp1); // Lower Left

        bool const alive = (cell.get_alive() && neighbors_alive == 2) || neighbors_alive == 3;
        _cells[new_index].set_alive(x, y, alive);
    }

    _index = new_index;
    return &_cells[_index];
}

inline size_t tomway::SimulationSystem::wrap(long long int val) const
{
    if (val < 0)
    {
        return _grid_size - 1;
    }
    
    return static_cast<size_t>(val) > _grid_size - 1 ? 0 : val;
}
