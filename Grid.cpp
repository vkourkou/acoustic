#include <Grid.h>

// -----------------------------------------------------------------------------

Grid1D::Grid1D(std::size_t size, Grid_t Min, Grid_t Delta)
    : m_data(size)
{
    if (!m_data.empty()) {
        m_data[0] = Min;
        for (std::size_t i = 1; i < size; ++i) {
            m_data[i] = m_data[i - 1] + Delta;
        }
    }
}

// -----------------------------------------------------------------------------

