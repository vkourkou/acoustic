#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <grid.h>
#include <input_compiler.h>
#include <utilities.h>
#include <dense_matrix.h>
#include <ostream>
#include <vector>

namespace FDTD {

// Forward declaration for CUDA workspace (defined in simulator_kernels.cuh)
struct CudaWorkSpace;
struct CudaWorkSpaceUnified;

class Simulator {
    class WorkSpace {
        DenseMatrix<float> m_Pres;
        DenseMatrix<float> m_Vx;
        DenseMatrix<float> m_Vy;
        void updateVx(float courantNb);
        void updateVy(float courantNb);
        void updatepressure(float crSquareTimesCourantNb);
    public:
        const DenseMatrix<float>& getPres() const {
            return m_Pres;
        }
        bool initialize(size_t numRows, size_t numCols);
        void updateFields(float courantNb, float crSquareTimesCourantNb);
        void UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val);
    };
    class WorkSpaceUnified {
        DenseMatrix<float> m_Pres;
        DenseMatrix<float> m_DeltaPres;
        void updateFields(float courantNb, float crSquareTimesCourantNb);
        void UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val);
    public:
        const DenseMatrix<float>& getPres() const {
            return m_Pres;
        }
        bool initialize(size_t numRows, size_t numCols);
        bool runBatch(size_t numIterations, float courantNb, float crSquareTimesCourantNb,
                      unsigned sourceGridX, unsigned sourceGridY,
                      const std::vector<float>& sourceValues);
    };
public:
    Simulator(const Input::BBoxStatement& Box, const Input::SourceStatement& Source, 
              const Input::VelocityStatement& Velocity, const Input::SimulationParamStatement& SimulationParam, 
              Dimension_t SpatialStep, Time_t TemporalStep, const std::string& dbFolderPath,
              const std::vector<Input::WallStatement>* walls);
    ~Simulator();
    
    void save(std::ostream& OS) const;
    Time_t getTime() const;
    
    bool execute();
private:
    Input::BBoxStatement m_Box;
    Input::SourceStatement m_Source;
    Input::VelocityStatement m_Velocity;
    Input::SimulationParamStatement m_SimulationParam;
    Dimension_t m_SpatialStep;
    Time_t m_TemporalStep;
    const std::vector<Input::WallStatement>* m_Walls;
    long m_GridDimPerStatialStep;
    float m_CourantNb{0.0e0};
    float m_Cr{1.0e0};
    float m_CrSquareTimesCourantNb{1.0e0};
    Grid2D m_Grids;
    WorkSpaceUnified m_WorkSpace;
    CudaWorkSpaceUnified* m_CudaWorkSpace;  // Only used when PT=GPU, managed in CUDA code
    unsigned m_SourceGridIndex_X{0};       
    unsigned m_SourceGridIndex_Y{0};        //The source is located at the center of the grid point
    size_t m_iteration{0};
    std::string m_dbFolderPath;
    //This function will find how many grid points are for each spatial step.
    long computeGridDimPerStatialStep(float MaxAlloweError) const;

    Grid_t computeDimensionGridSpace(Dimension_t Dimension) const;

    template<ProcessingType PT>
    bool runBatchIterations(size_t numIterations);

    template<ProcessingType PT>
    bool executeForType();

    template<ProcessingType PT>
    bool initializeMatrices();

    template<ProcessingType PT>
    bool runIterations();

    template<ProcessingType PT>
    bool potentiallySaveTheMatricesToDb();

    std::vector<float> computeSourceValues(size_t numIterations) const;

    void potentiallyTransferToDevice(DenseMatrix<float>& To);

};

} // namespace FDTD

#endif // SIMULATOR_H

