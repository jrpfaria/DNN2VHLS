#include "printtailor.h"

void* printtailor()
{
    FILE* fp;

    // Print .h file
    fp = fopen("datatailor.hpp", "w");
    fprintf(fp, "#include \"cnn.hpp\"\n\n");
    for(int layer = 1; layer <= nLayers; layer++)
        if (lTypes[thisLayer] == FC || lTypes[thisLayer] == CONV)
            for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
            {
                fprintf(fp, "modelData addBiasL%dK%d(", layer, kernel);
                for(int channel = 0; channel < nChannels[thisLayer]; channel++)
                    fprintf(fp, "const modelData in%d%s", channel, channel == nChannels[thisLayer]-1 ? ");\n" : ", ");    
            }

    for(int layer = 1; layer <= nLayers; layer++)
        if (aTypes[thisLayer] != NONE && (lTypes[thisLayer] == FC || lTypes[thisLayer] == CONV))
            for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
            {
                switch(aTypes[thisLayer])
                {
                    case RELU:
                        fprintf(fp, "modelData reluL%dK%d(const modelData in);\n", layer, kernel); break;
                    case ELU:
                        fprintf(fp, "modelData eluL%dK%d(const modelData in);\n", layer, kernel); break;
                    case LEAKY:
                        fprintf(fp, "modelData leakyL%dK%d(const modelData in);\n", layer, kernel); break;
                    case CLIPPED:
                        fprintf(fp, "modelData clippedL%dK%d(const modelData in);\n", layer, kernel); break;
                    case SWISH:
                        fprintf(fp, "modelData swishL%dK%d(const modelData in);\n", layer, kernel); break;
                    case TANH:
                        fprintf(fp, "modelData tanhL%dK%d(const modelData in);\n", layer, kernel); break;
                    case SIGMOID:
                        fprintf(fp, "modelData sigmoidL%dK%d(const modelData in);\n", layer, kernel); break;
                    case SOFTPLUS:
                        fprintf(fp, "modelData softplusL%dK%d(const modelData in);\n", layer, kernel); break;
                    case NONE:default:break;
                }
            }
    fclose(fp);

    // Print .c file
    fp = fopen("datatailor.cpp", "w");
    fprintf(fp, "#include \"datatailor.hpp\"\n\n");

    // Merge results and add biases
    for(int layer = 1; layer <= nLayers; layer++)
        if (lTypes[thisLayer] == FC || lTypes[thisLayer] == CONV)
            for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
            {
                fprintf(fp, "modelData addBiasL%dK%d(", layer, kernel);
                for(int channel = 0; channel < nChannels[thisLayer]; channel++)
                    fprintf(fp, "const modelData in%d%s", channel, channel == nChannels[thisLayer]-1 ? ")\n" : ", ");
                    
                fprintf(fp, "{\n");
#ifdef HLS
                fprintf(fp, "#pragma HLS PIPELINE II=%d\n", IIValue);
                fprintf(fp, "#pragma HLS INLINE off\n");    
                fprintf(fp, "#pragma HLS LATENCY min=0 max=0\n");
#endif
                fprintf(fp, "\tstatic const modelData bias = L%d_Bias%d;\n", layer, kernel);
                fprintf(fp, "\treturn ");
                for(int channel = 0; channel < nChannels[thisLayer]; channel++)
                    if (channel % 2 == 0)
                        fprintf(fp, "(in%d + ", channel);
                    else
                        fprintf(fp, "in%d) + ", channel);
                fprintf(fp, "bias%s;\n", nChannels[thisLayer] % 2 == 0 ? "" : ")");
                fprintf(fp, "}\n\n");
            }

    // Activation Functions
    for(int layer = 1; layer <= nLayers; layer++)
        if (aTypes[thisLayer] != NONE && (lTypes[thisLayer] == FC || lTypes[thisLayer] == CONV))    
            for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
            {
                switch(aTypes[thisLayer])
                {
                    case RELU:
                        fprintf(fp, "modelData reluL%dK%d(const modelData in)\n", layer, kernel); break;
                    case ELU:
                        fprintf(fp, "modelData eluL%dK%d(const modelData in)\n", layer, kernel); break;
                    case LEAKY:
                        fprintf(fp, "modelData leakyL%dK%d(const modelData in)\n", layer, kernel); break;
                    case CLIPPED:
                        fprintf(fp, "modelData clippedL%dK%d(const modelData in)\n", layer, kernel); break;
                    case SWISH:
                        fprintf(fp, "modelData swishL%dK%d(const modelData in)\n", layer, kernel); break;
                    case TANH:
                        fprintf(fp, "modelData tanhL%dK%d(const modelData in)\n", layer, kernel); break;
                    case SIGMOID:
                        fprintf(fp, "modelData sigmoidL%dK%d(const modelData in)\n", layer, kernel); break;
                    case SOFTPLUS:
                        fprintf(fp, "modelData softplusL%dK%d(const modelData in)\n", layer, kernel); break;
                    case NONE:default:break;
                }
                fprintf(fp, "{\n");
#ifdef HLS
                fprintf(fp, "#pragma HLS PIPELINE II=%d\n", IIValue);
                fprintf(fp, "#pragma HLS INLINE off\n");    
                fprintf(fp, "#pragma HLS LATENCY min=0 max=1\n");
#endif
                switch(aTypes[thisLayer])
                {
                    case RELU:
                        fprintf(fp, "\tmodelData valZero = 0;\n");
                        fprintf(fp, "\treturn in > valZero ? in : valZero;\n"); break;
                    case ELU:
                        fprintf(fp, "\tstatic const modelData alpha = L%d_AlphaK%d;\n", layer, kernel);
                        fprintf(fp, "\tmodelData valZero = 0;\n");
                        fprintf(fp, "\tmodelData in2 = in*in;\n");
                        fprintf(fp, "\tmodelData in3 = in*in*in;\n");
                        fprintf(fp, "\tmodelData ltZero = alpha * (in + in2/2 + in3/6);\n");
                        fprintf(fp, "\treturn in > valZero ? in : ltZero;\n"); break;
                    case LEAKY:
                        fprintf(fp, "\tmodelData valZero = 0;\n");
                        fprintf(fp, "\tmodelData alpha = L%d_AlphaK%d;\n", layer, kernel);
                        fprintf(fp, "\tmodelData ltZero = (alpha * in);\n");
                        fprintf(fp, "\treturn in > valZero ? in : ltZero;\n"); break;
                    case CLIPPED:
                        fprintf(fp, "\tmodelData valZero = 0;\n");
                        fprintf(fp, "\tmodelData top = L%d_TopK%d;\n", layer, kernel);
                        fprintf(fp, "\treturn in > valZero ? in < top ? in : top : valZero;\n"); break;
                    case SIGMOID:
                        fprintf(fp, "\tmodelData o5 = 0.5;\n");
                        fprintf(fp, "\treturn o5 + in/4 - (in*in*in)/48;\n"); break;
                    case SOFTPLUS:
                        fprintf(fp, "\tmodelData o7 = 0.693;\n");
                        fprintf(fp, "\tmodelData xby8 = in/8;\n");
                        fprintf(fp, "\treturn o7 + in/2 + in*xby8;\n"); break;
                    case SWISH:
                        fprintf(fp, "\tmodelData o5 = 0.5;\n");
                        fprintf(fp, "\treturn o5 + (in*in)/4 - (in*in)*(in*in)/48;\n"); break;
                    case TANH:
                        fprintf(fp, "\tstatic const modelData aux1 = 2/15;\n");
                        fprintf(fp, "\tstatic const modelData aux2 = 17/315;\n");
                        fprintf(fp, "\tmodelData in2 = in*in;\n");
                        fprintf(fp, "\tmodelData in3 = in*in*in;\n");
                        fprintf(fp, "\treturn in - (in3/3) + (aux1*(in3*in2)) - ((aux2*in3)*(in2*in2));\n"); break;
                    case NONE:default:break;
                }
                fprintf(fp, "}\n\n");
            }
    
    fclose(fp);

    pthread_exit(NULL);
}