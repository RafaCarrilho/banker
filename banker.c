#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "parser.h"


int main (int argc, char* argv[]){
    if (argc <=1){
        fprintf (stderr, "faltou argumento");
        return 1;
    }

    int recursos = argc-1;
    int* vetor = (int*) malloc (recursos* sizeof(int));
    
    if (vetor == NULL){
        fprintf (stderr, "malloc deu errado boy");
        return 1;
    }
    
    char* sobra;
    
    for (int i = 1; i<=recursos; i++){
        errno = 0;
        long int valor = strtol (argv[i], &sobra, 10);

        if (errno == ERANGE) {
            fprintf(stderr, "Erro: numero fora do intervalo permitido.\n");
            free(vetor);
            return 1;
        }

        if (sobra == argv[i]) {
            fprintf(stderr, "Erro: argumento sem numero.\n");
            free(vetor);
            return 1;
        }
        
        if (*sobra != '\0'){
            fprintf (stderr, "teve lixo no input");
            free(vetor);
            return 1;
        }

        if (valor < 0){
            fprintf (stderr, "teve lixo no input");
            free(vetor);
            return 1;
        }

        if (valor > INT_MAX){
            fprintf (stderr, "teve lixo no input");
            free(vetor);
            return 1;
        }

        vetor[i-1] = (int)valor;
        
    }

    FILE *arquivo = fopen("customer.txt", "r");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir customer.txt\n");
        free(vetor);
        return 1;
    }

    int clientes =0;
    
    char linha[256];
    while (1){
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        break;
        }

        clientes++;
    }
    printf("\nClientes : %d\n", clientes);
    fclose(arquivo);


    arquivo = fopen("customer.txt", "r");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir customer.txt\n");
        free(vetor);
        return 1;
    }

    int** matriz = (int**) malloc (clientes * sizeof(int*));
    if (matriz == NULL){
        fprintf (stderr, "malloc deu errado boy");
        free(vetor);
        fclose(arquivo);
        return 1;
    }

    for (int i=0; i < clientes; i++){
        matriz[i] = (int*) malloc (recursos * sizeof (int));
        
        if (matriz[i] == NULL){
            for (int j=0; j<i; j++){
                free (matriz[j]);
            }
            fprintf (stderr, "malloc deu errado boy");
            free(matriz);
            fclose(arquivo);
            free(vetor);
            return 1;
        }
    }
    
    for (int i = 0; i < clientes; i++) {
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {
            fprintf(stderr, "Erro ao ler cliente\n");

            fclose(arquivo);

            for (int j = 0; j < clientes; j++) {
                free(matriz[j]);
            }

            free(matriz);
            free(vetor);
            return 1;
        }

        if (parse(linha, matriz[i], recursos) != 0) {
            fprintf(stderr, "Erro nos dados do cliente %d\n", i);

            fclose(arquivo);

            for (int j = 0; j < clientes; j++) {
                free(matriz[j]);
            }

            free(matriz);
            free(vetor);

            return 1;
        }
    }

    for (int i = 0; i < clientes; i++) {
        printf("Cliente %d:", i);

        for (int j = 0; j < recursos; j++) {
            printf(" %d", matriz[i][j]);
        }

        printf("\n");
    }


    fclose(arquivo);
    for (int i = 0; i < clientes; i++) {
        free(matriz[i]);
    }
    free(matriz);
    free(vetor);

    return 0;
}