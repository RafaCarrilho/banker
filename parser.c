#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include "parser.h"

int parse(char *linha, int *destino, int recursos) {
    if (linha == NULL || destino == NULL || recursos <= 0) {
        return 1;
    }

    char *atual = linha;
    char *fim;

    for (int i = 0; i < recursos; i++) {
        errno = 0;
        long valor = strtol(atual, &fim, 10);

        if (fim == atual) {
            return 1;
        }

        if (errno == ERANGE || valor > INT_MAX || valor < 0) {
            return 1;
        }

        if (i < recursos - 1) {
            if (*fim != ',') {
                return 1;
            }
            atual = fim + 1;
        } else {
            if (*fim != '\n' && *fim != '\0') {
                return 1;
            }
        }

        destino[i] = (int)valor;
    }

    return 0;
}


int parse_comando(char *linha, char *tipo, int *cliente, int *pedido, int recursos) {

    char tipo_local[3];
    int cliente_local;

    if (linha == NULL || tipo == NULL ||
        cliente == NULL || pedido == NULL || recursos <= 0) {
        return 1;
    }

    char *token = strtok(linha, " \t\r\n"); //Primeira chamada pega o nome do comando

    if (token == NULL) {
        return 1;
    }

    if (strcmp(token, "RQ") == 0) {
        strcpy(tipo_local, "RQ");
    }
    else if (strcmp(token, "RL") == 0) {
        strcpy(tipo_local, "RL");
    }
    else if (strcmp(token, "*") == 0) {
        if (strtok(NULL, " \t\r\n") != NULL) {
            return 1;
        }

        strcpy(tipo, "*");
        return 0;
    }
    else {
        return 1;
    }

    //segunda chamada é pra achar o cliente
    char *t = strtok(NULL, " \t\r\n");

    if (t == NULL) {
        return 1;
    }

    char* sobra;
    errno = 0;
    long valor = strtol(t, &sobra, 10);
    
    if (sobra == t || *sobra != '\0') {
        return 1;
    }

    if (errno == ERANGE || valor > INT_MAX || valor < 0) {
        return 1;
    }

    cliente_local = (int)valor;


    //terceira pra fretne pega os recursos e guardarei num vetor

    int pedido_local[recursos];
    
    for (int i =0; i<recursos; i++){
        errno = 0;
        t= strtok(NULL, " \t\r\n");
        if (t==NULL){
            return 1;
        }
        valor = strtol(t, &sobra, 10);
        if (sobra == t || *sobra != '\0') {
            return 1;
        }

        if (errno == ERANGE || valor > INT_MAX || valor < 0) {
            return 1;
        }
        pedido_local[i] = (int)valor;

    }
    if (strtok(NULL, " \t\r\n") != NULL) {
        return 1;
    }

    strcpy(tipo, tipo_local);
    *cliente = cliente_local;

    for (int i = 0; i < recursos; i++) {
        pedido[i] = pedido_local[i];
    }

    return 0;
}