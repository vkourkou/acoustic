#make
#/usr/local/cuda-13.0//bin/ncu --set full --print-details=all ./acoustic sample_input.txt
export MAX_ITERATION=100
export PROCESSING_TYPE=GPU
if [ $# -ne 2 ]; then
    echo "Error: This script requires exactly 2 arguments"
    echo "Usage: $0  <BLOCK_X_SIZE> <BLOCK_Y_SIZE>"
    exit 1
fi

export BLOCK_X_SIZE=$1
export BLOCK_Y_SIZE=$2
rm -rf acoustic.db
file=profile_$1_$2.ncu-rep
outfile=profile_output_$1_$2.log
rm -rf profile.ncu-rep
rm -rf $file
rm -rf $outfile
/usr/local/cuda-13.0//bin/ncu -o profile --section SchedulerStats --section WarpStateStats --section SourceCounters --section SpeedOfLight --section SpeedOfLight_RooflineChart ./acoustic sample_input.txt | tee $outfile

mv profile.ncu-rep $file
