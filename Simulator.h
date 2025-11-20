#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <Grid.h>
#include <InputCompiler.h>
#include <Utilities.h>
#include <DenseMatrix.h>
#include <ostream>

namespace FDTD {

class Simulator {
public:
    Simulator(const Input::BBoxStatement& Box, const Input::SourceStatement& Source, const Input::VelocityStatement& Velocity, Dimension_t SpatialStep, Time_t TemporalStep);
    ~Simulator() = default;
    
    void save(std::ostream& OS) const;

    bool initializeMatrices();

private:
    Input::BBoxStatement m_Box;
    Input::SourceStatement m_Source;
    Input::VelocityStatement m_Velocity;
    Dimension_t m_SpatialStep;
    Time_t m_TemporalStep;
    long m_GridDimPerStatialStep;
    float m_CourantNb{0.0e0};
    Grid2D m_Grids;
    DenseMatrix<float> m_Pres;
    DenseMatrix<float> m_Vx;
    DenseMatrix<float> m_Vy;
    unsigned m_SourceGridIndex_X{0};       
    unsigned m_SourceGridIndex_Y{0};        //The source is located at the center of the grid point

    //This function will find how many grid points are for each spatial step.
    long computeGridDimPerStatialStep(float MaxAlloweError) const;

    Grid_t computeDimensionGridSpace(Dimension_t Dimension) const;

};

} // namespace FDTD

#endif // SIMULATOR_H

