all:
	gcc -Wall main.c validacao.c memoria.c -o vm

clean:
	rm -f vm