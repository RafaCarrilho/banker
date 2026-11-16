
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
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
