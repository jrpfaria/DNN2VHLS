#!/bin/bash

# echo "Generating the C++ Model Files..."

# Check if the user provided at least one argument
if [ -z "$1" ]; then
  echo "You can specify this parameters using:"
  echo "./generate.sh <FixedPointWidth[8;N]> [HLS] [DEBUG]"
  echo "Additionally, you can specify the PIPELINE flag to enable the pipeline pragma if generating files for HLS."
  gcc generator/*.c -o codegenerator -Wall -O3 -DFixedPointWidth=32
else
  FixedPointWidth=$1
  
  # echo "FixedPointWidth: $FixedPointWidth"
  
  # Base GCC command
  FLAGS="-DFixedPointWidth=$FixedPointWidth"

  # Execute compilation
  gcc generator/*.c -o codegenerator -Wall -O3 -DHLS -DPIPELINE $FLAGS
fi

# Check compilation success
if [ $? -ne 0 ]; then
  echo "Compilation failed. Please check the generator.c file."
  exit 1
fi

# Generate the C files for the model
./codegenerator

# Check execution success
if [ $? -ne 0 ]; then
  echo "Error while running the generator."
  exit 1
fi

rm -f codegenerator
#echo "Execution completed successfully! You can now run the run.sh script to compile and execute the generated C program."
