
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include "parser.h"
#include "seguranca.h"


static void liberar_matriz(int **m, int n){
    if (m == NULL){
        return;
    }

    for (int i=0; i<n; i++){
        free(m[i]);
    }

    free(m);
}


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
            liberar_matriz(maximum, i);
            fprintf (stderr, "malloc deu errado boy");
            fclose(arquivo);
            free(vetor);
            return 1;
        }
    }

    for (int i = 0; i < num_clientes; i++) {
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {
            fprintf(stderr, "Erro ao ler cliente\n");

            fclose(arquivo);

            liberar_matriz(maximum, num_clientes);
            free(vetor);
            return 1;
        }

        if (parse(linha, maximum[i], recursos) != 0) {
            fprintf(stderr, "Erro nos dados do cliente %d\n", i);

            fclose(arquivo);

            liberar_matriz(maximum, num_clientes);
            free(vetor);

            return 1;
        }
    }

    fclose(arquivo);

    int** allocation = (int**) malloc (num_clientes * sizeof(int*));
    if (allocation == NULL){
        fprintf (stderr, "malloc deu errado boy");

        liberar_matriz(maximum, num_clientes);
        free(vetor);
        return 1;
    }

    for (int i=0; i < num_clientes; i++){
        allocation[i] = (int*) calloc (recursos, sizeof (int));

        if (allocation[i] == NULL){
            liberar_matriz(allocation, i);

            fprintf (stderr, "malloc deu errado boy");

            liberar_matriz(maximum, num_clientes);
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

        liberar_matriz(maximum, num_clientes);
        liberar_matriz(allocation, num_clientes);
        free(vetor);
        return 1;
    }

    FILE *resultado_arquivo = fopen("result.txt", "w");

    if (resultado_arquivo == NULL){
        fprintf(stderr, "Erro ao abrir result.txt\n");

        fclose(comandos);
        liberar_matriz(maximum, num_clientes);
        liberar_matriz(allocation, num_clientes);
        free(vetor);

        return 1;
    }
    //leitura geral
    while (1) {
        if (fgets(linha, sizeof(linha), comandos) == NULL) {
            break;
        }
        char tipo[3];
        int cliente;
        int pedido[recursos];
        if (parse_comando(linha, tipo, &cliente, pedido, recursos) != 0) {
            fprintf(stderr, "Erro ao interpretar comando\n");

            fclose(resultado_arquivo);
            fclose(comandos);

            liberar_matriz(maximum, num_clientes);
            liberar_matriz(allocation, num_clientes);
            free(vetor);

            return 1;
        }
        
        if (tipo[0] != '*' && cliente >= num_clientes){  //verificação se não tem cliente a mais do que devia
            fprintf(stderr, "Cliente invalido: %d\n", cliente);
            continue;
        }

        printf("Comando interpretado: %s\n", tipo);

        
        //parte do RQ
        
        if (strcmp(tipo, "RQ") == 0){
            int excedeu = 0;

            for (int i =0; i<recursos; i++){
                if (pedido[i] > maximum[cliente][i] - allocation[cliente][i]){
                    excedeu = 1;
                }
            }

            if (excedeu == 1){
                printf("Pedido excede o limite do cliente\n");
                continue;
            }

            
            int insuficiente = 0;

            for (int i =0; i<recursos; i++){
                if (pedido[i] > vetor[i]){
                    insuficiente = 1;
                    break;
                }
            }

            if (insuficiente == 1){
                printf("Recursos insuficientes\n");
                continue;
            }

            for (int i =0; i<recursos; i++){
                vetor[i] -= pedido[i];
                allocation[cliente][i] += pedido[i];
            }
            int resultado = verifica(maximum, allocation, vetor, num_clientes, recursos);

            if (resultado != 0){
                for (int i =0; i<recursos; i++){
                    vetor[i] += pedido[i];
                    allocation[cliente][i] -= pedido[i];
                }

                if (resultado == 1){
                    printf("Pedido negado: estado inseguro\n");
                    continue;
                }

                if (resultado == 2){
                    fprintf(stderr, "Erro de memoria na verificacao\n");

                    fclose(resultado_arquivo);
                    fclose(comandos);
                    liberar_matriz(maximum, num_clientes);
                    liberar_matriz(allocation, num_clientes);
                    free(vetor);

                    return 1;
                }
            }

            printf("Pedido aprovado\n");


        }
        //parte do RL
        if (strcmp(tipo, "RL") == 0){
            int excedeu = 0;

            for (int i =0; i<recursos; i++){
                if (pedido[i] > allocation[cliente][i]){
                    excedeu = 1;
                    break;
                }
            }

            if (excedeu == 1){
                printf("Cliente tentando liberar mais recursos do que possui\n");
                continue;
            }

            for (int i =0; i<recursos; i++){
                allocation[cliente][i] -= pedido[i];
                vetor[i] += pedido[i];
            }

            printf("Recursos liberados\n");
        }

        
        
        //escrita no arquivo, parte *
        if (strcmp(tipo, "*") == 0){
            fprintf(resultado_arquivo, "MAXIMUM   |   ALLOCATION  |   NEED\n");

            for (int i =0; i<num_clientes; i++){
                for (int j =0; j<recursos; j++){
                    fprintf(resultado_arquivo, "%d ", maximum[i][j]);
                }

                fprintf(resultado_arquivo, "    |   ");

                for (int j =0; j<recursos; j++){
                    fprintf(resultado_arquivo, "%d ", allocation[i][j]);
                }

                fprintf(resultado_arquivo, "      |   ");

                for (int j =0; j<recursos; j++){
                    fprintf(resultado_arquivo, "%d ", maximum[i][j] - allocation[i][j]);
                }

                fprintf(resultado_arquivo, "\n");
            }

            fprintf(resultado_arquivo, "AVAILABLE ");

            for (int i =0; i<recursos; i++){
                fprintf(resultado_arquivo, "%d ", vetor[i]);
            }

            fprintf(resultado_arquivo, "\n");
        }

    }

    fclose(resultado_arquivo);
    fclose(comandos);
    liberar_matriz(maximum, num_clientes);
    free(vetor);

    liberar_matriz(allocation, num_clientes);

    return 0;
}
