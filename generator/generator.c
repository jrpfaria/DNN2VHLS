#include "generator.h"

int main()
{
    int nThreads = 5;
    pthread_t threads[nThreads];
    double delta_time;

    //  We choose to generate the various functions over different files
    //  that way we can use parallel threads to generate them concurrently without any problem

    (void) get_delta_time ();

    pthread_create(&threads[0], NULL, printcomputation, NULL);
    pthread_create(&threads[1], NULL, printcontrol, NULL);
    pthread_create(&threads[2], NULL, printtailor, NULL);
    pthread_create(&threads[3], NULL, printmodel, NULL);
    pthread_create(&threads[4], NULL, printdata, NULL);
    
    delta_time = get_delta_time ();

    for (int i = 0; i < nThreads; i++)
        pthread_join(threads[i], NULL);
    
    // printf("Files generated successfully!\n");
    printf("Elapsed time: %.6f seconds\n", delta_time);

    return 0;
}

