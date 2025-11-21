#!/usr/bin/env python3
"""
Script to read DenseMatrix files and create colormap visualizations.
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
import os
import glob
from pathlib import Path


def read_dense_matrix(filename):
    """
    Read a DenseMatrix file and return metadata and numpy array.
    
    Format:
    - First line: "Time = value" (title for the plot)
    - Second line: x-axis tick values (space-separated numbers)
    - Third line: y-axis tick values (space-separated numbers)
    - Fourth line: number of rows and columns (space-separated)
    - Following lines: space-separated values for each row
    
    Args:
        filename: Path to the DenseMatrix file
        
    Returns:
        tuple: (title, x_ticks, y_ticks, matrix) where matrix is a numpy array
    """
    with open(filename, 'r') as f:
        # Read header lines
        title_line = f.readline().strip()
        x_ticks = [float(x) for x in f.readline().strip().split()]
        y_ticks = [float(x) for x in f.readline().strip().split()]
        
        # Read dimensions (rows and cols on the same line, separated by space)
        dims = f.readline().strip().split()
        rows = int(dims[0])
        cols = int(dims[1])
        
        # Read matrix data
        data = []
        for i in range(rows):
            line = f.readline().strip()
            if not line:
                raise ValueError(f"Expected {rows} rows but got {i} rows")
            row_values = [float(x) for x in line.split()]
            if len(row_values) != cols:
                raise ValueError(f"Row {i} has {len(row_values)} columns, expected {cols}")
            data.append(row_values)
    
    return (title_line, x_ticks, y_ticks, np.array(data))


def create_colormap(matrix, output_filename=None, title=None, x_ticks=None, y_ticks=None):
    """
    Create a colormap visualization of a matrix.
    
    Args:
        matrix: numpy array containing the matrix data
        output_filename: Optional filename to save the figure
        title: Title for the plot
        x_ticks: List of tick values for the x-axis
        y_ticks: List of tick values for the y-axis
    """
    fig, ax = plt.subplots(figsize=(10, 8))
    
    # Create colormap
    im = ax.imshow(matrix, cmap='viridis', aspect='equal', origin='lower')
    
    # Add colorbar
    cbar = plt.colorbar(im, ax=ax)
    cbar.set_label('Value', rotation=270, labelpad=20)
    
    # Set ticks and labels
    #print(f"x_ticks: {x_ticks}")
    #print(f"y_ticks: {y_ticks}")
    if x_ticks and len(x_ticks) == matrix.shape[1]:
        # Set x-axis ticks at pixel positions with custom labels
        # Limit to at most 10 ticks
        MaxNumTicksX = 10
        num_x_ticks = min(MaxNumTicksX, len(x_ticks))
        if num_x_ticks == len(x_ticks):
            tick_positions_x = np.arange(matrix.shape[1])
            tick_labels_x = [f'{val:.3g}' for val in x_ticks]
        else:
            # Select evenly spaced indices
            indices = np.linspace(0, len(x_ticks) - 1, num_x_ticks, dtype=int)
            tick_positions_x = indices
            tick_labels_x = [f'{x_ticks[i]:.3g}' for i in indices]
        ax.set_xticks(tick_positions_x)
        ax.set_xticklabels(tick_labels_x)
        ax.set_xlabel('X')
    else:
        raise ValueError(f"Not correct x_ticks")
    
    if y_ticks and len(y_ticks) == matrix.shape[0]:
        # Set y-axis ticks at pixel positions with custom labels
        # Limit to at most 10 ticks
        MaxNumTicksY = 10
        num_y_ticks = min(MaxNumTicksY, len(y_ticks))
        if num_y_ticks == len(y_ticks):
            tick_positions_y = np.arange(matrix.shape[0])
            tick_labels_y = [f'{val:.3g}' for val in y_ticks]
        else:
            # Select evenly spaced indices
            indices = np.linspace(0, len(y_ticks) - 1, num_y_ticks, dtype=int)
            tick_positions_y = indices
            tick_labels_y = [f'{y_ticks[i]:.3g}' for i in indices]
        ax.set_yticks(tick_positions_y)
        ax.set_yticklabels(tick_labels_y)
        ax.set_ylabel('Y')
    else:
        raise ValueError(f"Not correct x_ticks")
    
    if title:
        ax.set_title(title)
    else:
        ax.set_title('Matrix Colormap')
    
    # Add grid for better readability
    ax.grid(True, alpha=0.3, linestyle='--', linewidth=0.5)
    
    plt.tight_layout()
    
    if output_filename:
        plt.savefig(output_filename, dpi=150, bbox_inches='tight')
        print(f"Saved colormap to {output_filename}")
    else:
        plt.show()
    
    plt.close()


def main():
    """
    Main function to process command line arguments and visualize matrices.
    """
    if len(sys.argv) < 2:
        print("Usage: python visualize_matrix.py <matrix_file> [output_image]")
        print("   or: python visualize_matrix.py --all [output_dir]")
        print("\nExamples:")
        print("  python visualize_matrix.py iteration_10.db")
        print("  python visualize_matrix.py iteration_10.db output.png")
        print("  python visualize_matrix.py --all")
        print("  python visualize_matrix.py --all ./output_images")
        sys.exit(1)
    
    if sys.argv[1] == '--all':
        # Process all .db files in the current directory
        db_files = glob.glob("*.db")
        if not db_files:
            print("No .db files found in current directory")
            sys.exit(1)
        
        output_dir = sys.argv[2] if len(sys.argv) > 2 else "."
        os.makedirs(output_dir, exist_ok=True)
        
        for db_file in sorted(db_files):
            try:
                print(f"Processing {db_file}...")
                title, x_ticks, y_ticks, matrix = read_dense_matrix(db_file)
                
                # Create output filename
                base_name = Path(db_file).stem
                output_file = os.path.join(output_dir, f"{base_name}.png")
                
                create_colormap(matrix, output_file, title=title, x_ticks=x_ticks, y_ticks=y_ticks)
            except Exception as e:
                print(f"Error processing {db_file}: {e}")
                continue
        
        print(f"\nProcessed {len(db_files)} files")
    else:
        # Process single file
        input_file = sys.argv[1]
        output_file = sys.argv[2] if len(sys.argv) > 2 else None
        
        if not os.path.exists(input_file):
            print(f"Error: File '{input_file}' not found")
            sys.exit(1)
        
        try:
            title, x_ticks, y_ticks, matrix = read_dense_matrix(input_file)
            print(f"Loaded matrix: {matrix.shape[0]} rows x {matrix.shape[1]} columns")
            print(f"Value range: [{matrix.min():.6f}, {matrix.max():.6f}]")
            
            create_colormap(matrix, output_file, title=title, x_ticks=x_ticks, y_ticks=y_ticks)
        except Exception as e:
            print(f"Error: {e}")
            sys.exit(1)


if __name__ == "__main__":
    main()

