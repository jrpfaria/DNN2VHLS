#!/bin/bash

# Execute compilation
gcc generator/*.c -o codegenerator -Wall -O3 -DHLS -DPIPELINE $FLAGS

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
