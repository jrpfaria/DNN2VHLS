#include "printcontrol.h"

void* printcontrol()
{
    FILE* fp;

    // Print .h file
    fp = fopen("datacontrol.hpp", "w");
    fprintf(fp, "#include \"cnn.hpp\"\n\n");
    for(int layer = 1; layer <= nLayers; layer++)
        if (lTypes[thisLayer] != FC && kSizes[thisLayer] != 1)
            for(int channel = 0; channel < nChannels[thisLayer]; channel++)
                fprintf(fp, "char controlL%dData_Ch%d(const modelData in, L%d_Window* out);\n", layer, channel, layer);
    fclose(fp);

    // Print .c file
    fp = fopen("datacontrol.cpp", "w");
    fprintf(fp, "#include \"datacontrol.hpp\"\n\n");
    for(int layer = 1; layer <= nLayers; layer++)
        if (lTypes[thisLayer] != FC && kSizes[thisLayer] != 1)
            for(int channel = 0; channel < nChannels[thisLayer]; channel++)
            {
                fprintf(fp, "char controlL%dData_Ch%d(const modelData in, L%d_Window* out)\n", layer, channel, layer);
                fprintf(fp, "{\n");
#ifdef HLS
                fprintf(fp, "#pragma HLS PIPELINE II=%d\n", IIValue);
                fprintf(fp, "#pragma HLS INLINE off\n");
#endif
                fprintf(fp, "\tstatic modelData data[L%d_PaddedFrameWidth * L%d_PaddedFrameHeight] = {0};\n", layer, layer);
#ifdef HLS
                fprintf(fp, "#pragma HLS BIND_STORAGE variable=data type=ram_1wnr\n");
#endif
                fprintf(fp, "\tstatic int wIdX = L%d_Padding, wIdY = L%d_Padding;\n", layer, layer);
                fprintf(fp, "\tstatic int rIdX = 0, rIdY = 0;\n");
                fprintf(fp, "\tstatic char dataFull = 0;\n");
                fprintf(fp, "\tstatic char valid = 0;\n");
                fprintf(fp, "\t\tL%d_Window dOut;\n\n", layer);
                fprintf(fp, "\tif (!dataFull)\n");
                fprintf(fp, "\t{\n");
                fprintf(fp, "\t\tdata[wIdY * L%d_PaddedFrameWidth + wIdX] = in;\n", layer);
                fprintf(fp, "\t\twIdX += L%d_hStride;\n", layer);
                fprintf(fp, "\t\tif (wIdX >= L%d_PaddedFrameWidth - L%d_Padding)\n", layer, layer);
                fprintf(fp, "\t\t\twIdX = L%d_Padding, wIdY += L%d_vStride;\n", layer, layer);
                fprintf(fp, "\t\tif (wIdY >= L%d_PaddedFrameHeight - L%d_Padding)\n", layer, layer);
                fprintf(fp, "\t\t\twIdX = L%d_Padding, wIdY = L%d_Padding, dataFull = 1;\n", layer, layer);
                fprintf(fp, "\t}\n\n");
                fprintf(fp, "\tvalid = (rIdY < wIdY - L%d_kHeight + 1 || (rIdY == wIdY - L%d_kHeight + 1 && rIdX < wIdX - L%d_kWidth + 1)) ? 1 : dataFull;\n\n", layer, layer, layer);
                fprintf(fp, "\tif (valid)\n");
                fprintf(fp, "\t{\n");
#ifdef HLS
                fprintf(fp, "copy:\n{\n");
                fprintf(fp, "#pragma HLS LATENCY min=0 max=1\n");
#endif

                for(int row = 0; row < kHeights[thisLayer]; row++)
                    for(int col = 0; col < kWidths[thisLayer]; col++)
                        fprintf(fp, "\t\tdOut.id%d = data[(rIdY + %d) * L%d_PaddedFrameWidth + rIdX + %d];\n", row*kWidths[thisLayer] + col, row, layer, col);
                fprintf(fp, "\t\t*out = dOut;\n");
#ifdef HLS
                fprintf(fp, "}\n");
#endif
                fprintf(fp, "\t\trIdX += L%d_hStride;\n", layer);
                fprintf(fp, "\t\tif (rIdX >= L%d_PaddedFrameWidth - L%d_kWidth + 1)\n", layer, layer);
                fprintf(fp, "\t\t\trIdX = 0, rIdY += L%d_vStride;\n", layer);
                fprintf(fp, "\t\tif (rIdY >= L%d_PaddedFrameHeight - L%d_kHeight + 1)\n", layer, layer);
                fprintf(fp, "\t\t\trIdX = 0, rIdY = 0, dataFull = 0;\n");
                fprintf(fp, "\t}\n\n");
                fprintf(fp, "\treturn valid;\n");
                fprintf(fp, "}\n\n");
            }
        
    fclose(fp);

    pthread_exit(NULL);
}
