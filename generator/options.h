typedef enum { FC, CONV, MAXP, MINP, AVGP } enumLayerTypes;
typedef enum { RELU, ELU, LEAKY, CLIPPED, SWISH, SOFTPLUS, TANH, SIGMOID, NONE } enumActivationTypes;

#include "../data/Kernels.h"
#include "../data/Biases.h"

// Number of layers
#define nLayers 3
#define layerTypes {CONV, CONV, CONV}
#define activationTypes {RELU, RELU, NONE}

// Number of kernels and channels per layer
#define kernelsPerLayer {2, 4, 1}
#define channelsPerLayer {1, 2, 4}

// Padding for the layers
#define L1_Padding 4
#define L2_Padding 1
#define L3_Padding 1
#define Paddings {L1_Padding, L2_Padding, L3_Padding}

// Kernel dimensions
#define L1_kWidth 5
#define L1_kHeight 5
#define L1_kSize (L1_kWidth * L1_kHeight)
#define L2_kWidth 5
#define L2_kHeight 5
#define L2_kSize (L2_kWidth * L2_kHeight)
#define L3_kWidth 5
#define L3_kHeight 5
#define L3_kSize (L3_kWidth * L3_kHeight)

#define kernelWidths {L1_kWidth, L2_kWidth, L3_kWidth}
#define kernelHeights {L1_kHeight, L2_kHeight, L3_kHeight}
#define kernelSizes {L1_kSize, L2_kSize, L3_kSize}

// Strides for the layers
#define L1_vStride 1
#define L2_vStride 1
#define L3_vStride 1
#define vStrides {L1_vStride, L2_vStride, L3_vStride}
#define L1_hStride 1
#define L2_hStride 1
#define L3_hStride 1
#define hStrides {L1_hStride, L2_hStride, L3_hStride}

// Input frame dimensions
#define InputWidth 14
#define InputHeight 288

// Fixed point width and decimal point width
#ifndef FixedPointWidth
#define FixedPointWidth 32
#endif
#define IntegerPointWidth 4

#ifndef IIValue
#define IIValue 1
#endif
