#ifndef PARSER_H
#define PARSER_H


int parse (char *linha, int *destino, int recursos);
int parse_comando(char *linha, char *tipo, int *cliente, int *pedido, int recursos);

#endif