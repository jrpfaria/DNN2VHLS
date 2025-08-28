#include "printcomputation.h"

void* printcomputation()
{
    FILE* fp;
    
    // Print .h file
    fp = fopen("computations.hpp", "w");
    fprintf(fp, "#include \"cnn.hpp\"\n\n");
    for(int layer = 1; layer <= nLayers; layer++)
        for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
            for(int channel = 0; channel < nChannels[thisLayer]; channel++)
                switch(lTypes[thisLayer])
                {
                    case MAXP:
                        if (kernel == 0)
                            fprintf(fp, "modelData maxPoolingLayer%d_Ch%d(const L%d_Window in);\n", layer, channel, layer);
                        break;
                    case MINP:
                        if (kernel == 0)
                            fprintf(fp, "modelData minPoolingLayer%d_Ch%d(const L%d_Window in);\n", layer, channel, layer);
                        break;
                    case AVGP:
                        if (kernel == 0)
                            fprintf(fp, "modelData averagePoolingLayer%d_Ch%d(const L%d_Window in);\n", layer, channel, layer);
                        break;
                    case FC:
                        fprintf(fp, "modelData fullyConnectedLayer%d_Kernel%d_Ch%d(const modelData in);\n", layer, kernel, channel);
                        break;
                    case CONV:
                    default:
                        fprintf(fp, "modelData convoluteLayer%d_Kernel%d_Ch%d(const L%d_Window in);\n", layer, kernel, channel, layer);
                        break;
                }
    fclose(fp);

    // Print .c file
    fp = fopen("computations.cpp", "w");
    fprintf(fp, "#include \"computations.hpp\"\n\n");
    for(int layer = 1; layer <= nLayers; layer++)
        for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
            if ((lTypes[thisLayer] != MAXP && lTypes[thisLayer] != MINP && lTypes[thisLayer] != AVGP) || kernel == 0)
                
                for(int channel = 0; channel < nChannels[thisLayer]; channel++)
                {
                    switch(lTypes[thisLayer])
                    {
                        case MAXP:
                            fprintf(fp, "modelData maxPoolingLayer%d_Ch%d(const L%d_Window in)\n", layer, channel, layer);
                            break;
                        case MINP:
                            fprintf(fp, "modelData minPoolingLayer%d_Ch%d(const L%d_Window in)\n", layer, channel, layer);
                            break;
                        case AVGP:
                            fprintf(fp, "modelData averagePoolingLayer%d_Ch%d(const L%d_Window in)\n", layer, channel, layer);
                            break;
                        case FC:
                            fprintf(fp, "modelData fullyConnectedLayer%d_Kernel%d_Ch%d(const modelData in)\n", layer, kernel, channel);
                            break;
                        case CONV:
                        default:
                            fprintf(fp, "modelData convoluteLayer%d_Kernel%d_Ch%d(const L%d_Window in)\n", layer, kernel, channel, layer);
                            break;
                    }
                    fprintf(fp, "{\n");

#ifdef HLS
                    fprintf(fp, "#pragma HLS PIPELINE II=%d\n", IIValue);
                    fprintf(fp, "#pragma HLS INLINE off\n");
#endif
                    if(lTypes[thisLayer] == CONV || lTypes[thisLayer] == FC)
                        fprintf(fp, "\tstatic const modelData f[L%d_kSize] = L%d_Kernel%dC%d;\n", layer, layer, kernel, channel);
#ifdef HLS
                    if(lTypes[thisLayer] == CONV)
                        fprintf(fp, "#pragma HLS ARRAY_PARTITION variable=f dim=1 type=complete\n");
                    fprintf(fp, "hiddenlayeroperation:\n{\n");
                    fprintf(fp, "#pragma HLS LATENCY min=0 max=1\n");
#endif
                    // Algorithm
                    switch(lTypes[thisLayer])
                    {
                        case MAXP:
                            fprintf(fp, "\tmodelData max = in.id0;\n");
                            fprintf(fp, "\treturn ");
                            for(int i = 1; i < kSizes[thisLayer]; i++)
                                fprintf(fp, "\tmax < in.id%d ? in.id%d :%s%s", i, i, i == kSizes[thisLayer] - 1 ? " max;" : "\n", i == kSizes[thisLayer] - 1 ? "" : "\t\t");
                            break;
                        case MINP:
                            fprintf(fp, "\tmodelData min = in.id0;\n");
                            fprintf(fp, "\treturn ");
                            for(int i = 1; i < kSizes[thisLayer]; i++)
                                fprintf(fp, "\tmin > in.id%d ? in.id%d :%s%s", i, i, i == kSizes[thisLayer] - 1 ? " min;" : "\n", i == kSizes[thisLayer] - 1 ? "" : "\t\t");
                            break;
                        case AVGP:
                            fprintf(fp, "\treturn (");
                            for(int i = 0; i < kSizes[thisLayer]; i++)
                            {
                                fprintf(fp, "in.id%d", i);
                                if (i == kSizes[thisLayer] - 1)
                                    fprintf(fp, ") / (L%d_kSize);", layer);
                                else
                                    fprintf(fp, " + ");
                            }
                            break;
                        case FC:
                            fprintf(fp, "\treturn in * f;");
                            break;
                        case CONV:
                        default:
                            for(int i = 0; i < kSizes[thisLayer]; i++)
                                fprintf(fp, "\tmodelData p%d = in.id%d * f[%d];\n", i, i, i);
                            fprintf(fp, "\treturn ");
                            for(int i = 0; i < kSizes[thisLayer]; i++)
                                fprintf(fp, "p%d%s", i, i == kSizes[thisLayer] - 1 ? ";" : " + ");
                            break;
                    }
#ifdef HLS
                    fprintf(fp, "\n}\n");
#endif

                    fprintf(fp, "}\n\n");
                }
    fclose(fp);

    pthread_exit(NULL);
}
