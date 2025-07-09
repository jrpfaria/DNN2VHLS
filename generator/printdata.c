#include "printdata.h"

void* printdata()
{
    FILE* fp;

    fp = fopen("cnn.hpp", "w");

    fprintf(fp, "#pragma once\n");
    fprintf(fp, "#include \"ap_fixed.h\"\n\n");
    fprintf(fp, "typedef ap_fixed<%d, %d, AP_RND_CONV, AP_WRAP> modelData;\n\n", FixedPointWidth, IntegerPointWidth);

    for (int layer = 1; layer <= nLayers; layer++)
        if (lTypes[thisLayer] != FC)
        {    
            fprintf(fp, "typedef struct L%d_Window {\n\tmodelData ", layer);
            for (int row = 0; row < kHeights[thisLayer]; row++)
                for (int col = 0; col < kWidths[thisLayer]; col++)
                {
                    fprintf(fp, "%sid%d", (col == 0 && row != 0) ? "\t\t\t  ":"", row * kWidths[thisLayer] + col);
                    if (col == kWidths[thisLayer] - 1)
                        fprintf(fp, "%s\n", (row == kHeights[thisLayer] - 1) ? ";" : ",");
                    else
                        fprintf(fp, ", ");
                }
            fprintf(fp, "} L%d_Window;\n\n", layer);
        }   
    

    fprintf(fp, "#include \"data/datain.h\"\n");
    fprintf(fp, "#include \"data/dataout.h\"\n");

    fprintf(fp, "\n");

    fprintf(fp, "#include \"data/Kernels.h\"\n");
    fprintf(fp, "#include \"data/Biases.h\"\n");
    
    for (int layer = 1; layer <= nLayers; layer++)
        if (aTypes[thisLayer] == ELU || aTypes[thisLayer] == LEAKY)
        {
            fprintf(fp, "#include \"data/Alphas.h\"\n");
            break;
        }

    fprintf(fp, "\n");

    fprintf(fp, "// Padding for the layers\n");

    for (int layer = 1; layer <= nLayers; layer++)
    {
        fprintf(fp, "#define L%d_Padding %d\n", layer, Padding[thisLayer]);
    }
    
    fprintf(fp, "// Kernel dimensions\n");
    for (int layer = 1; layer <= nLayers; layer++)
    {
        if (lTypes[thisLayer] != FC)
        {
            fprintf(fp, "#define L%d_kWidth  %d\n", layer, kWidths[thisLayer]);
            fprintf(fp, "#define L%d_kHeight %d\n", layer, kHeights[thisLayer]);
            fprintf(fp, "#define L%d_kSize   (L%d_kWidth * L%d_kHeight)\n", layer, layer, layer);
        }
    }

    fprintf(fp, "// Strides for the layers\n");
    for (int layer = 1; layer <= nLayers; layer++)
    {
        fprintf(fp, "#define L%d_hStride %d\n", layer, hStride[thisLayer]);
        fprintf(fp, "#define L%d_vStride %d\n", layer, vStride[thisLayer]);
    }

    fprintf(fp, "// Frame dimensions across the layers\n");
    fprintf(fp, "#define InputWidth  %d\n#define InputHeight %d\n\n", InputWidth, InputHeight);
    fprintf(fp, "#define L1_InputWidth  InputWidth\n#define L1_InputHeight InputHeight\n\n");

    fprintf(fp, "#define futureDim(x,p,k,s)    (((x)+2*(p)-((k)-1))/s)\n");

    fprintf(fp, "#define futureWidth(x,p,k,s)  futureDim(x,p,k,s)\n");
    fprintf(fp, "#define futureHeight(x,p,k,s) futureDim(x,p,k,s)\n\n");

    for (int layer = 2; layer <= nLayers; layer++)
    {
        switch(lTypes[thisLayer])
        {
            case FC:
                fprintf(fp, "#define L%d_InputWidth  L%d_InputWidth\n", layer, thisLayer);
                fprintf(fp, "#define L%d_InputHeight L%d_InputHeight\n\n", layer, thisLayer);
                break;
            case MAXP:case AVGP:case CONV:default:
                fprintf(fp, "#define L%d_InputWidth  futureWidth(L%d_InputWidth, L%d_Padding, L%d_kWidth, L%d_hStride)\n", layer, thisLayer, thisLayer, thisLayer, thisLayer);
                fprintf(fp, "#define L%d_InputHeight futureHeight(L%d_InputHeight, L%d_Padding, L%d_kHeight, L%d_vStride)\n\n", layer, thisLayer, thisLayer, thisLayer, thisLayer);
                break;
            
        }
    }

    fprintf(fp, "#define ResultWidth  futureWidth(L%d_InputWidth, L%d_Padding, L%d_kWidth, L%d_hStride)\n", nLayers, nLayers, nLayers, nLayers);
    fprintf(fp, "#define ResultHeight futureHeight(L%d_InputHeight, L%d_Padding, L%d_kHeight, L%d_vStride)\n\n", nLayers, nLayers, nLayers, nLayers);

    for (int layer = 1; layer <= nLayers; layer++)
    {
        fprintf(fp, "#define L%d_PaddedFrameWidth  (L%d_InputWidth + 2*L%d_Padding)\n", layer, layer, layer);
        fprintf(fp, "#define L%d_PaddedFrameHeight (L%d_InputHeight + 2*L%d_Padding)\n\n", layer, layer, layer);
    }

    fclose(fp);

    pthread_exit(NULL);
}