#ifndef GRID_H
#define GRID_H

#include <vector>
#include <cstddef>
#include <cassert>

typedef int Grid_t;

class Grid1D {
private:
    std::vector<Grid_t> m_data;

public:
    // Constructor with size, Min, and Delta
    Grid1D(std::size_t size, Grid_t Min, Grid_t Delta);

    std::size_t size() const {
        return m_data.size();
    }

    const Grid_t& operator[](std::size_t index) const {
        assert(index < m_data.size());;
        return m_data[index];
    }

    bool empty() const {
        return m_data.empty();
    }

};

#endif // GRID_H

