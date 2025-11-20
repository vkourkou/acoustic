#include <InputCnt.h>
#include <iostream>

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

Dimension_t
InputCnt::computeWavelength() const
{
    Velocity_t velocity = get<Input::VelocityStatement>().getVelocity();
    Frequency_t frequency = get<Input::SourceStatement>().getFreq();
    return static_cast<Dimension_t>(velocity / frequency);
}

// -----------------------------------------------------------------------------

bool
InputCnt::isSane() const
{
    bool AllSane = get<Input::SourceStatement>().isValid() && get<Input::BBoxStatement>().isValid() &&
                   get<Input::VelocityStatement>().isValid() && get<Input::MaxResolutionStatement>().isValid();
    if (!AllSane) {
        return false;
    }
    if (!get<Input::BBoxStatement>().isPointStrictlyInside(get<Input::SourceStatement>().getX(), get<Input::SourceStatement>().getY())) {
        std::cout << "Source is not strictly inside the bounding box\n";
        return false;
    }
    return true;
}

}

// -----------------------------------------------------------------------------

