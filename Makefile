.PHONY: clean
banker: banker.c parser.c parser.h seguranca.c seguranca.h
	gcc -g -Wall -Wextra -std=c11 banker.c parser.c seguranca.c -o banker

clean:
	rm -f banker