#include "config.h"

const int nKernels[nLayers] = kernelsPerLayer;
const int nChannels[nLayers] = channelsPerLayer;
const int Padding[nLayers] = Paddings;
const int vStride[nLayers] = vStrides;
const int hStride[nLayers] = hStrides;
const int kWidths[nLayers] = kernelWidths;
const int kHeights[nLayers] = kernelHeights;
const int kSizes[nLayers] = kernelSizes;
const enumLayerTypes lTypes[nLayers] = layerTypes;
const enumActivationTypes aTypes[nLayers] = activationTypes;