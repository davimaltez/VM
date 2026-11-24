all:
	gcc -Wall main.c validacao.c memoria.c tlb.c politica.c -o vm

clean:
	rm -f vm