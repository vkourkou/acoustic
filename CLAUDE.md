# Acoustic FDTD Simulator — Claude Context

## Project Overview

2D acoustic wave simulator using the Finite-Difference Time-Domain (FDTD) scheme. Supports CPU and GPU (NVIDIA CUDA) execution. Started as a CUDA/GPU learning project — some design choices are intentionally experimental.

## Language & Build

- **Languages:** C++17, CUDA 17
- **Build system:** CMake 3.20+
- **Testing:** Google Test (fetched automatically via FetchContent)

### Build commands

```bash
# Quick build (recommended)
./compile.sh

# CMake directly
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Debug with CUDA debugging
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_CUDA_DEBUG=ON
cmake --build build
```

### Outputs

- `build/acoustic` — main simulation executable
- `build/ut` — unit test executable

## Running

```bash
# Direct
./build/acoustic sample_input.txt

# Via helper script
./run.sh <MAX_ITERATION> <PROCESSING_TYPE> <BLOCK_X_SIZE> <BLOCK_Y_SIZE>
# e.g.: ./run.sh 1000 GPU 16 16
```

Input files support `$VAR` environment variable expansion (see `sample_input.txt`).

## Testing

```bash
cd build && ./ut        # run unit tests directly
cd build && ctest       # via CTest
```

Tests cover: input parsing, utilities, grid logic (6 test suites, CPU only — no CUDA in `ut`).

## Key Source Files

| File | Role |
|------|------|
| `main.cpp` | Entry point |
| `runner.{h,cpp}` | Orchestrates parse → validate → execute |
| `simulator.{h,cpp}` | FDTD time-stepping loop, CPU/GPU dispatch |
| `grid.{h,cpp}` | 2D spatial grid |
| `input_file_parser.{h,cpp}` | Low-level tokenization |
| `input_compiler.{h,cpp}` | Structured statement parsing |
| `input_cnt.{h,cpp}` | Parsed input container |
| `simulator_kernels.{cu,cuh}` | CUDA kernels (pressure/velocity updates) |
| `cuda_utilities.{cu,cuh}` | GPU device queries, memory management |
| `cuda_array.h` | GPU (device) array abstraction |
| `cuda_pinned_array.h` | Pinned host array abstraction (for async transfers) |
| `cuda_dense_matrix.h` | GPU matrix abstraction |
| `dense_matrix.h` | CPU matrix abstraction |
| `utilities.{h,cpp}` | Timing, misc helpers |

## Input File Format

Plain text, keyword-based. Key statement types:
- `Source` — acoustic source (frequency, amplitude, duration)
- `BBox` — bounding box / domain extents
- `Wall` — obstacle regions
- `Velocity` — custom velocity profiles
- `SimulationParam` — execution mode (CPU/GPU), block dimensions, iteration count
- `MaxResolution` — spatial discretization

## Output & Visualization

- Simulator writes binary `.db` files into `acoustic.db/` directory
- `compare_out.py <ref.db> <candidate.db>` — compare two outputs (CPU vs GPU validation)
- `create_colormap.py <file.db>` — visualize pressure field as colormap image

## Validation

After any code change:

1. Run unit tests:
```bash
cd build && ./ut
```
All 626 tests must pass.

2. Run the GPU simulation and verify output is bit-identical to the saved reference:
```bash
./run.sh 1000 GPU 16 16
diff GPU.1000/iteration_1000.db save.1000/iteration_1000.db
```
The `diff` must produce no output. `GPU.1000/` is written by `run.sh`; `save.1000/` is the committed reference.

## Benchmarking / Profiling

Uses NVIDIA Nsight Compute (`ncu`):

```bash
./benchmark.sh <BLOCK_X> <BLOCK_Y>           # single config
./benchmark_all.sh                            # sweep over many configs → bench_results/
```

## TODO (from project)

- Explore multi-step kernel launches to increase GPU utilization (currently under-utilized for 2D domain)
- Investigate CUDA graphs to further reduce CPU↔GPU control switching per timestep (stream-based async already implemented)
- Move run-time block parameters to compile-time higher in the hierarchy

## Platform

Primary: Linux x86_64 with NVIDIA GPU + CUDA Toolkit. Windows scripts exist but are not actively maintained.
