#include <InputCnt.h>

namespace input {

// -----------------------------------------------------------------------------

void
InputCnt::save(std::ostream& OS) const
{
    saveImpl(OS, std::make_index_sequence<std::tuple_size_v<StatementsTuple>>{});
}

// -----------------------------------------------------------------------------

Time_t
InputCnt::computeTimeStep() const
{
    Frequency_t frequency = get<Input::SourceStatement>().getFreq();
    float period = 1.0f / frequency;
    float temporal = get<Input::MaxResolutionStatement>().getTemporal();
    return static_cast<Time_t>(period * temporal);
}

// -----------------------------------------------------------------------------

Dimension_t
InputCnt::computeSpatialStep() const
{
    Velocity_t velocity = get<Input::VelocityStatement>().getVelocity();
    Frequency_t frequency = get<Input::SourceStatement>().getFreq();
    float wavelength = velocity / frequency;
    float spatial = get<Input::MaxResolutionStatement>().getSpatial();
    return static_cast<Dimension_t>(wavelength * spatial);
}

// -----------------------------------------------------------------------------

}
