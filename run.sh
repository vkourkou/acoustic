
if [ $# -ne 4 ]; then
    echo "Error: This script requires exactly 4 arguments"
    echo "Usage: $0 <MAX_ITERATION> <PROCESSING_TYPE> <BLOCK_X_SIZE> <BLOCK_Y_SIZE>"
    exit 1
fi
#Possible usage ./run.sh 1000 GPU 16 16 //will run 1000 time steps with GPU and block size 16x16
export MAX_ITERATION=$1
export PROCESSING_TYPE=$2
export BLOCK_X_SIZE=$3
export BLOCK_Y_SIZE=$4
./acoustic sample_input.txt | tee output.log
save_dir=$PROCESSING_TYPE.$MAX_ITERATION
rm -rf $save_dir
mv acoustic.db $save_dir
mv output.log $save_dir


