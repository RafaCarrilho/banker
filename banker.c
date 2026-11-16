#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>


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
}