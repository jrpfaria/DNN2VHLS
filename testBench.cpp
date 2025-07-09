#include <stdio.h>
#include <unistd.h>
#include "model.hpp"

int main()
{
    char valid;
    modelData out[1] = {0};

    modelData cnnResults[ResultWidth * ResultHeight] = {0};
    modelData cnnData[InputWidth * InputHeight] = dataIn;
    modelData expectedResults[ResultWidth * ResultHeight] = dataOut;

    double goldenValue = 0; // Update this value after first execution in order to utilize the C/RTL synthesis

    int write_counter = 0;
    int received_counter = 0;
    for (;received_counter < ResultHeight * ResultWidth;)
    {
        valid = model(&(cnnData[(write_counter++) % (InputHeight*InputWidth)]), out);
        if (valid)
        {
            cnnResults[received_counter] = *out;
            received_counter++;
        }
    }
    
    // print matrix of results
    double meanError = 0;
    for (int i = 0; i < ResultHeight; i++)
        for (int j = 0; j < ResultWidth; j++)
            meanError += (double)
                         ((cnnResults[i * ResultWidth + j] - expectedResults[i * ResultWidth + j]) 
                         *(cnnResults[i * ResultWidth + j] - expectedResults[i * ResultWidth + j]));
    meanError /= (ResultHeight * ResultWidth);
    printf("Mean squared error: %100.98lf\n", meanError);

    return !(meanError == goldenValue);
}
