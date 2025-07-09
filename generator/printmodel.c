#include "printmodel.h"

void* printmodel()
{
    FILE* fp;

    fp = fopen("model.hpp", "w");
    fprintf(fp, "#include \"cnn.hpp\"\n");
    fprintf(fp, "#include \"computations.hpp\"\n");
    fprintf(fp, "#include \"datacontrol.hpp\"\n");
    fprintf(fp, "#include \"datatailor.hpp\"\n\n");

    fprintf(fp, "char model(modelData in[1], ");
    for (int kernel = 0; kernel < nKernels[nLayers-1]; kernel++)
        fprintf(fp, "modelData out%d[1]%s", kernel, kernel == nKernels[nLayers-1] - 1 ? ");\n" : ", ");

    fclose(fp);

    fp = fopen("model.cpp", "w");
    fprintf(fp, "#include \"model.hpp\"\n\n");

    fprintf(fp, "char model(modelData in[1], ");
    for (int kernel = 0; kernel < nKernels[nLayers-1]; kernel++)
        fprintf(fp, "modelData out%d[1]%s", kernel, kernel == nKernels[nLayers-1] - 1 ? ")\n" : ", ");
    fprintf(fp, "{\n");
    
#ifdef HLS
    fprintf(fp, "#pragma HLS INTERFACE port=in mode=ap_vld\n");
    for (int kernel = 0; kernel < nKernels[nLayers-1]; kernel++)
        fprintf(fp, "#pragma HLS INTERFACE port=out%d mode=ap_ovld\n", kernel);
    fprintf(fp, "#pragma HLS INTERFACE port=return mode=ap_none\n");
#ifdef PIPELINE
    fprintf(fp, "#pragma HLS PIPELINE II=%d\n\n", IIValue);
#else 
    fprintf(fp, "\n");
#endif
#endif

    int identationLevel = 0;

    // Declare variables
    for(int layer = 1; layer <= nLayers; layer++)
    {
        fprintf(fp, "// Layer %d Variables\n", layer);
        for(int channel = 0; channel < nChannels[thisLayer]; channel++)
            if (lTypes[thisLayer] != FC)
            {
                if (channel == 0)
                    fprintf(fp, "\tL%d_Window ", layer);
                fprintf(fp, "L%d_CalculationData%d[1]%s", layer, channel, (channel == nChannels[thisLayer] - 1) ? ";\n" : ", ");
            }
        for(int channel = 0; channel < nChannels[thisLayer]; channel++)
            for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
                if ((lTypes[thisLayer] == MAXP || lTypes[thisLayer] == MINP || lTypes[thisLayer] == AVGP))
                {   
                    if (kernel == 0)
                        fprintf(fp, "\tmodelData L%d_ResultsC%d;\n", layer, channel);
                }
                else
                    fprintf(fp, "%sL%d_Results%dC%d%s", (kernel == 0) ? "\tmodelData ":"", layer, kernel, channel, (kernel == nKernels[thisLayer] - 1) ? ";\n" : ", ");
        if (lTypes[thisLayer] != MAXP && lTypes[thisLayer] != MINP && lTypes[thisLayer] != AVGP)
        {
            for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
                fprintf(fp, "%sL%d_Forward%d%s", (kernel == 0) ? "\tmodelData ":"", layer, kernel, (kernel == nKernels[thisLayer] - 1) ? ";\n" : ", ");
            for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
                if (aTypes[thisLayer] != NONE)
                    fprintf(fp, "%sL%d_Out%d%s", (kernel == 0) ? "\tmodelData ":"", layer, kernel, (kernel == nKernels[thisLayer] - 1) ? ";\n" : ", ");    
        }
        for(int channel = 0; channel < nChannels[thisLayer]; channel++)
            if (lTypes[thisLayer] != FC)
                fprintf(fp, "%sL%d_Valid%d%s", (channel == 0) ? "\tchar ":"", layer, channel, (channel == nChannels[thisLayer] - 1) ? ";\n" : ", ");
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");

    char input[50];
    char auxinput[50];
    char computation[100];
    char activation[100];
    char output[50];

    // Algorithm
    for(int layer = 1; layer <= nLayers; layer++)
    {
        fprintf(fp, "// Layer %d\n", layer);

        // DRBs
        for(int channel = 0, kernel = 0; channel < nChannels[thisLayer]; channel++)
        {
            if (lTypes[thisLayer] != FC)
            {
                for(int aux = 0; aux < identationLevel; aux++)
                    fprintf(fp, "\t");
                if (layer > 1)
                    switch(lTypes[thisLayer-1])
                    {
                        case MAXP:
                        case MINP:
                        case AVGP:
                            sprintf(input, "L%d_Results%dC%d", layer-1, kernel, channel);
                            break;
                        default:
                            sprintf(input, layer == 1 ? "*in" : "L%d_%s%d", thisLayer, (aTypes[thisLayer-1] != NONE) ? "Out" : "Forward", kernel++);
                            break;
                    }
                else
                    sprintf(input, layer == 1 ? "*in" : "L%d_%s%d", thisLayer, (aTypes[thisLayer] != NONE) ? "Out" : "Forward", kernel++);        
                fprintf(fp, "\tL%d_Valid%d = controlL%dData_Ch%d(%s, L%d_CalculationData%d);\n", layer, channel, layer, channel, input, layer, channel);
            }
        }
        
        if (lTypes[thisLayer] != FC)
        {
            for(int aux = 0; aux < identationLevel; aux++)
                fprintf(fp, "\t");
            fprintf(fp, "\tif (");

            for(int channel = 0; channel < nChannels[thisLayer]; channel++)
            {
                if (channel % 2 == 0 && channel + 1 <= nChannels[thisLayer] - 1)
                    fprintf(fp, "(");
                fprintf(fp, "L%d_Valid%d%s%s", layer, channel, channel%2 == 1 ? ")":"", channel == nChannels[thisLayer]-1 ? ")\n" : " && ");
            }

            for(int aux = 0; aux < identationLevel; aux++)
                fprintf(fp, "\t");
            fprintf(fp, "\t{\n");
            identationLevel++;
        }

        // Layer Operations
        for(int kernel = 0; kernel < nKernels[thisLayer]; kernel++)
        {
            if ((lTypes[thisLayer] != MAXP && lTypes[thisLayer] != MINP && lTypes[thisLayer] != AVGP) || kernel == 0)
                // Computations
                for(int channel = 0; channel < nChannels[thisLayer]; channel++)
                {
                    for(int aux = 0; aux < identationLevel; aux++)
                        fprintf(fp, "\t");
                    switch(lTypes[thisLayer])
                    {
                        case MAXP:
                                sprintf(computation, "maxPoolingLayer");
                                sprintf(auxinput, "*L%d_CalculationData%d", layer, channel);
                                fprintf(fp, "\tL%d_ResultsC%d = %s%d_Ch%d(%s);\n", layer, channel, computation, layer, channel, auxinput);
                            break;
                        case MINP:
                                sprintf(computation, "minPoolingLayer");
                                sprintf(auxinput, "*L%d_CalculationData%d", layer, channel);
                                fprintf(fp, "\tL%d_ResultsC%d = %s%d_Ch%d(%s);\n", layer, channel, computation, layer, channel, auxinput);
                            break;
                        case AVGP:
                                sprintf(computation, "averagePoolingLayer");
                                sprintf(auxinput, "*L%d_CalculationData%d", layer, channel);
                                fprintf(fp, "\tL%d_ResultsC%d = %s%d_Ch%d(%s);\n", layer, channel, computation, layer, channel, auxinput);
                            break;
                        case FC:
                            sprintf(computation, "fullyConnectedLayer");
                            if (layer > 1)
                                switch (lTypes[thisLayer-1])
                                {
                                    case MAXP:
                                    case MINP:
                                    case AVGP:
                                        sprintf(auxinput, "L%d_ResultsC%d", layer-1, channel);
                                        break;
                                    default:
                                        sprintf(auxinput, layer == 1 ? "*in" : "L%d_%s%d", thisLayer, (aTypes[thisLayer] != NONE) ? "Out" : "Forward", channel);
                                        break;
                                }
                            else
                                sprintf(auxinput, layer==1 ? "*in" : "L%d_%s%d", thisLayer, (aTypes[thisLayer] != NONE) ? "Out" : "Forward", channel);
                            fprintf(fp, "\tL%d_Results%dC%d = %s%d_Kernel%d_Ch%d(%s);\n", layer, kernel, channel, computation, layer, kernel, channel, auxinput);
                            break;
                        case CONV:
                        default:
                            sprintf(computation, "convoluteLayer");
                            sprintf(auxinput, "*L%d_CalculationData%d", layer, channel);
                            fprintf(fp, "\tL%d_Results%dC%d = %s%d_Kernel%d_Ch%d(%s);\n", layer, kernel, channel, computation, layer, kernel, channel, auxinput);
                            break;
                    }
                }

            if (lTypes[thisLayer] != MAXP && lTypes[thisLayer] != MINP && lTypes[thisLayer] != AVGP)
            {
                // Add Bias
                for(int aux = 0; aux < identationLevel; aux++)
                    fprintf(fp, "\t");
                if (layer == nLayers && aTypes[thisLayer] == NONE)
                    sprintf(output, "*out%d", kernel);
                else
                    sprintf(output, "L%d_Forward%d", layer, kernel);
                fprintf(fp, "\t%s = addBiasL%dK%d(", output, layer, kernel);
                for(int aux = 0; aux < nChannels[thisLayer]; aux++)
                    fprintf(fp, "L%d_Results%dC%d%s", layer, kernel, aux, aux == nChannels[thisLayer]-1 ? ");\n" : ", ");
            

                // Activation Functions
                if (aTypes[thisLayer] != NONE)
                {
                    for(int aux = 0; aux < identationLevel; aux++)
                        fprintf(fp, "\t");
                    switch(aTypes[thisLayer])
                    {
                        case RELU: sprintf(activation, "relu"); break;
                        case ELU: sprintf(activation, "elu"); break;
                        case LEAKY: sprintf(activation, "leaky"); break;
                        case CLIPPED: sprintf(activation, "clipped"); break;
                        case SIGMOID: sprintf(activation, "sigmoid"); break;
                        case TANH: sprintf(activation, "tanh"); break;
                        case SWISH: sprintf(activation, "swish"); break;
                        case SOFTPLUS: sprintf(activation, "softplus"); break;
                        case NONE:default:break;
                    }
                    if (layer == nLayers)
                        sprintf(output, "*out%d", kernel);
                    else
                        sprintf(output, "L%d_Out%d", layer, kernel);
                    fprintf(fp, "\t%s = %sL%dK%d(L%d_Forward%d);\n", output, activation, layer, kernel, layer, kernel);
                }
            }

            if (layer == nLayers && kernel == nKernels[thisLayer] - 1)
            {
                for(int aux = 0; aux < identationLevel; aux++)
                    fprintf(fp, "\t");
                fprintf(fp, "\treturn 1;\n");
            }
        }
        fprintf(fp, "// End of Layer %d\n%s", layer, layer == nLayers ? "" : "\n");
    }

    int maxIdentationLevel = identationLevel;

    // Close Parentheses
    for(int layer = 1; layer <= nLayers; layer++)
    {
        identationLevel--;
        if (lTypes[thisLayer] != FC)
        {
            for(int aux = 0; aux < identationLevel; aux++)
                fprintf(fp, "\t");
            fprintf(fp, "\t}\n");
        }
    }


    if (maxIdentationLevel) fprintf(fp, "\n\treturn 0;\n");
    fprintf(fp, "}\n");


    fclose(fp);

    pthread_exit(NULL);
}