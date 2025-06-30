#pragma once

// Common Includes
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "options.h"

// Auxiliary macros and functions
#define thisLayer (layer - 1)

extern const int nKernels[nLayers];
extern const int nChannels[nLayers];
extern const int Padding[nLayers];
extern const int vStride[nLayers];
extern const int hStride[nLayers];
extern const int kWidths[nLayers];
extern const int kHeights[nLayers];
extern const int kSizes[nLayers];
extern const enumLayerTypes lTypes[nLayers];
extern const enumActivationTypes aTypes[nLayers];
