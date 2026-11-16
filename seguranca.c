#include <stdio.h>
#include <stdlib.h>
#include "seguranca.h"

int verifica (int** maximum, int** allocation,int* available, int clientes, int recursos){
    int* work = (int*) malloc (recursos * sizeof (int));
    if (work == NULL){
        fprintf(stderr, "Erro no work\n");
        return 2;
    }

    int* finish =(int*) calloc (clientes, sizeof(int));
    if (finish == NULL){
        fprintf(stderr, "Erro no finish\n");
        free(work);
        return 2;
    }

    for (int i=0; i<recursos; i++){
        work[i] = available[i];
    }


    int terminados =0;

    while (terminados < clientes){
        int flag = 0;

        for (int i=0; i<clientes; i++){
            if (finish[i] != 1){
                int cabe = 1;
                for (int j=0; j<recursos; j++){
                    if ((maximum[i][j] - allocation[i][j])>work[j]){
                        cabe =0;
                    }
                }
                if (cabe == 1){
                    flag =1;
                    terminados ++;
                    finish[i] = 1;
                    for (int j=0; j<recursos; j++){
                        work[j] += allocation[i][j];
                    }
                }
            }
        }


        if (flag == 0){
            free (work);
            free (finish);
            return 1;
        }
    }
    free (work);
    free (finish);
    return 0;
}
