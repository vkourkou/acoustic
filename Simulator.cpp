#include <Simulator.h>
#include <cmath>
#include <cstddef>

// -----------------------------------------------------------------------------
namespace FDTD {

Simulator::Simulator(const Input::BBoxStatement& Box, const Input::SourceStatement& Source, const Input::VelocityStatement& Velocity, Dimension_t SpatialStep, Time_t TemporalStep)
    :
    m_Box(Box),
    m_Source(Source),
    m_Velocity(Velocity),
    m_SpatialStep(SpatialStep),
    m_TemporalStep(TemporalStep),
    m_Grids(
        static_cast<std::size_t>(std::ceil((Box.getXMax() - Box.getXMin()) / SpatialStep) + 1),
        static_cast<Grid_t>(Box.getXMin()),
        static_cast<Grid_t>(SpatialStep),
        static_cast<std::size_t>(std::ceil((Box.getYMax() - Box.getYMin()) / SpatialStep) + 1),
        static_cast<Grid_t>(Box.getYMin()   ),
        static_cast<Grid_t>(SpatialStep)
    )
    //FIXME: The above has to become more robust
{
}

// -----------------------------------------------------------------------------

void
Simulator::save(std::ostream& OS) const
{
    m_Box.save(OS);
    OS << "\n";
    m_Source.save(OS);
    OS << "\n";
    m_Velocity.save(OS);
    OS << "\n";
    OS << "SpatialStep " << m_SpatialStep << "\n";
    OS << "TemporalStep " << m_TemporalStep << "\n";
    OS << "Grids: " << "\n";
    m_Grids.save(OS);
}

// -----------------------------------------------------------------------------

}