all:
	gcc -Wall -pthread -DTLB_THREADS main.c validacao.c memoria.c tlb.c politica.c -o vm

sem_thread:
	gcc -Wall -pthread main.c validacao.c memoria.c tlb.c politica.c -o vm

sem_tlb:
	gcc -Wall -pthread -DSEM_TLB main.c validacao.c memoria.c tlb.c politica.c -o vm

sem_tlb_256:
	gcc -Wall -pthread -DSEM_TLB -DTOTAL_FRAMES=256 main.c validacao.c memoria.c tlb.c politica.c -o vm