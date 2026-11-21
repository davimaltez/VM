all:
	gcc -Wall main.c validacao.c -o vm

clean:
	rm -f vm