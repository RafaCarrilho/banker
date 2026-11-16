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

    int num_clientes =0;

    char linha[256];
    while (1){
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        break;
        }

        num_clientes++;
    }
    printf("\nClientes : %d\n", num_clientes);
    fclose(arquivo);


    arquivo = fopen("customer.txt", "r");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir customer.txt\n");
        free(vetor);
        return 1;
    }

    int** maximum = (int**) malloc (num_clientes * sizeof(int*));
    if (maximum == NULL){
        fprintf (stderr, "malloc deu errado boy");
        free(vetor);
        fclose(arquivo);
        return 1;
    }

    for (int i=0; i < num_clientes; i++){
        maximum[i] = (int*) malloc (recursos * sizeof (int));

        if (maximum[i] == NULL){
            for (int j=0; j<i; j++){
                free (maximum[j]);
            }
            fprintf (stderr, "malloc deu errado boy");
            free(maximum);
            fclose(arquivo);
            free(vetor);
            return 1;
        }
    }

    for (int i = 0; i < num_clientes; i++) {
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {
            fprintf(stderr, "Erro ao ler cliente\n");

            fclose(arquivo);

            for (int j = 0; j < num_clientes; j++) {
                free(maximum[j]);
            }

            free(maximum);
            free(vetor);
            return 1;
        }

        if (parse(linha, maximum[i], recursos) != 0) {
            fprintf(stderr, "Erro nos dados do cliente %d\n", i);

            fclose(arquivo);

            for (int j = 0; j < num_clientes; j++) {
                free(maximum[j]);
            }

            free(maximum);
            free(vetor);

            return 1;
        }
    }

    fclose(arquivo);

    int** allocation = (int**) malloc (num_clientes * sizeof(int*));
    if (allocation == NULL){
        fprintf (stderr, "malloc deu errado boy");

        for (int i=0; i<num_clientes; i++){
            free(maximum[i]);
        }
        free(maximum);
        free(vetor);
        return 1;
    }

    for (int i=0; i < num_clientes; i++){
        allocation[i] = (int*) calloc (recursos, sizeof (int));

        if (allocation[i] == NULL){
            for (int j=0; j<i; j++){
                free (allocation[j]);
            }

            fprintf (stderr, "malloc deu errado boy");
            free(allocation);

            for (int j=0; j<num_clientes; j++){
                free(maximum[j]);
            }

            free(maximum);
            free(vetor);
            return 1;
        }
    }

    for (int i = 0; i < num_clientes; i++) {
        printf("Cliente %d:", i);

        for (int j = 0; j < recursos; j++) {
            printf(" %d", maximum[i][j]);
        }

        printf("\n");
    }

    FILE *comandos = fopen("commands.txt", "r");

    if (comandos == NULL) {
        fprintf(stderr, "Erro ao abrir commands.txt\n");

        for (int i = 0; i < num_clientes; i++) {
            free(maximum[i]);
        }

        free(maximum);
        for (int i = 0; i < num_clientes; i++) {
            free(allocation[i]);
        }
        free(allocation);
        free(vetor);
        return 1;
    }

    while (1) {
        if (fgets(linha, sizeof(linha), comandos) == NULL) {
            break;
        }
        char tipo[3];
        int cliente;
        int pedido[recursos];
        if (parse_comando(linha, tipo, &cliente, pedido, recursos) != 0) {
            fprintf(stderr, "Erro ao interpretar comando\n");

            fclose(comandos);

            for (int i = 0; i < num_clientes; i++) {
                free(maximum[i]);
            }

            free(maximum);
            for (int i = 0; i < num_clientes; i++) {
                free(allocation[i]);
            }
            free(allocation);
            free(vetor);

            return 1;
        }

        printf("Comando interpretado: %s\n", tipo);

        if (tipo[0] != '*') {
            printf("Cliente: %d | Recursos:", cliente);

            for (int i = 0; i < recursos; i++) {
                printf(" %d", pedido[i]);
            }

            printf("\n");
        }

    }

    fclose(comandos);
    for (int i = 0; i < num_clientes; i++) {
        free(maximum[i]);
    }
    free(maximum);
    free(vetor);

    for (int i = 0; i < num_clientes; i++) {
        free(allocation[i]);
    }
    free(allocation);

    return 0;
}