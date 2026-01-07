#!/usr/bin/env python3
"""
Script to compare two database files produced by Simulator::potentiallySaveTheMatricesToDb().
Checks if header information matches and computes relative error statistics for the data.
"""

import sys
import numpy as np
from pathlib import Path


def parse_file(filepath):
    """Parse a database file and return header info and data array."""
    with open(filepath, 'r') as f:
        lines = f.readlines()
    
    # Parse line 1: Time
    time_line = lines[0].strip()
    if not time_line.startswith("Time:"):
        raise ValueError(f"Expected 'Time:' at start of line 1, got: {time_line}")
    time_value = float(time_line.split(":", 1)[1].strip())
    
    # Parse line 2: Grid X (space-separated values)
    grid_x = np.array([float(x) for x in lines[1].strip().split()])
    
    # Parse line 3: Grid Y (space-separated values)
    grid_y = np.array([float(y) for y in lines[2].strip().split()])
    
    # Parse line 4: Dimensions (rows cols)
    dim_line = lines[3].strip().split()
    if len(dim_line) != 2:
        raise ValueError(f"Expected 2 values on line 4 (rows cols), got: {dim_line}")
    rows, cols = int(dim_line[0]), int(dim_line[1])
    
    # Parse remaining lines: pressure matrix data
    data_lines = lines[4:]
    data_values = []
    for line in data_lines:
        line = line.strip()
        if line:  # Skip empty lines
            data_values.extend([float(x) for x in line.split()])
    
    # Convert to numpy array
    data_array = np.array(data_values)
    
    # Verify expected size
    expected_size = rows * cols
    if len(data_array) != expected_size:
        raise ValueError(f"Expected {expected_size} values, got {len(data_array)}")
    
    return {
        'time': time_value,
        'grid_x': grid_x,
        'grid_y': grid_y,
        'rows': rows,
        'cols': cols,
        'data': data_array
    }


def compare_files(file1_path, file2_path):
    """Compare two database files and report differences."""
    print(f"Comparing files:")
    print(f"  File 1: {file1_path}")
    print(f"  File 2: {file2_path}")
    print()
    
    # Parse both files
    try:
        file1 = parse_file(file1_path)
        file2 = parse_file(file2_path)
    except Exception as e:
        print(f"Error parsing files: {e}")
        return False
    
    # Compare header information
    differences = []
    
    # Check time
    if abs(file1['time'] - file2['time']) > 1e-10:
        differences.append(f"Time mismatch: {file1['time']} vs {file2['time']}")
    
    # Check grid X
    if not np.allclose(file1['grid_x'], file2['grid_x'], rtol=1e-10, atol=1e-10):
        differences.append("Grid X mismatch")
        print(f"Grid X lengths: {len(file1['grid_x'])} vs {len(file2['grid_x'])}")
    
    # Check grid Y
    if not np.allclose(file1['grid_y'], file2['grid_y'], rtol=1e-10, atol=1e-10):
        differences.append("Grid Y mismatch")
        print(f"Grid Y lengths: {len(file1['grid_y'])} vs {len(file2['grid_y'])}")
    
    # Check dimensions
    if file1['rows'] != file2['rows'] or file1['cols'] != file2['cols']:
        differences.append(f"Dimensions mismatch: {file1['rows']}x{file1['cols']} vs {file2['rows']}x{file2['cols']}")
    
    # Report header differences
    if differences:
        print("Header differences found:")
        for diff in differences:
            print(f"  - {diff}")
        return False
    
    print("Header information matches!")
    print(f"  Time: {file1['time']}")
    print(f"  Grid X size: {len(file1['grid_x'])}")
    print(f"  Grid Y size: {len(file1['grid_y'])}")
    print(f"  Pressure matrix dimensions: {file1['rows']} x {file1['cols']}")
    print()
    
    # Compare data arrays
    data1 = file1['data']
    data2 = file2['data']
    
    if len(data1) != len(data2):
        print(f"Data size mismatch: {len(data1)} vs {len(data2)}")
        return False
    
    # Compute relative errors
    # Avoid division by zero by using absolute error when reference is near zero
    mask_nonzero = np.abs(data1) > 1e-10
    relative_errors = np.zeros_like(data1)
    
    # For non-zero values, compute relative error
    relative_errors[mask_nonzero] = np.abs((data1[mask_nonzero] - data2[mask_nonzero]) / data1[mask_nonzero])
    
    # For near-zero values, use absolute error
    mask_zero = ~mask_nonzero
    if np.any(mask_zero):
        relative_errors[mask_zero] = np.abs(data1[mask_zero] - data2[mask_zero])
    
    # Compute statistics
    min_error = np.min(relative_errors)
    max_error = np.max(relative_errors)
    avg_error = np.mean(relative_errors)
    std_error = np.std(relative_errors)
    
    print("Relative Error Statistics:")
    print(f"  Minimum relative error: {min_error:.10e} ({min_error * 100:.4f}%)")
    print(f"  Maximum relative error: {max_error:.10e} ({max_error * 100:.4f}%)")
    print(f"  Average relative error:  {avg_error:.10e} ({avg_error * 100:.4f}%)")
    print(f"  Std deviation:          {std_error:.10e} ({std_error * 100:.4f}%)")
    
    # Additional statistics
    median_error = np.median(relative_errors)
    p95_error = np.percentile(relative_errors, 95)
    p99_error = np.percentile(relative_errors, 99)
    
    print()
    print("Additional Statistics:")
    print(f"  Median relative error:   {median_error:.10e} ({median_error * 100:.4f}%)")
    print(f"  95th percentile:        {p95_error:.10e} ({p95_error * 100:.4f}%)")
    print(f"  99th percentile:        {p99_error:.10e} ({p99_error * 100:.4f}%)")
    
    # Count of values with significant differences
    threshold = 1e-6
    significant_diffs = np.sum(relative_errors > threshold)
    print(f"  Values with error > {threshold}: {significant_diffs} ({100.0 * significant_diffs / len(relative_errors):.2f}%)")
    
    return True


def main():
    if len(sys.argv) != 3:
        print("Usage: python compare_db_files.py <file1.db> <file2.db>")
        sys.exit(1)
    
    file1_path = Path(sys.argv[1])
    file2_path = Path(sys.argv[2])
    
    if not file1_path.exists():
        print(f"Error: File not found: {file1_path}")
        sys.exit(1)
    
    if not file2_path.exists():
        print(f"Error: File not found: {file2_path}")
        sys.exit(1)
    
    success = compare_files(file1_path, file2_path)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()

