#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>

#define TAMANHO_PAGINA 256
#define TOTAL_PAGINAS 256
#define TOTAL_FRAMES 128

typedef struct Endereco {
    int pagina;
    int offset;
}Endereco;

typedef struct {
    int anterior;
    int proximo;
} Elo;


typedef struct GerenciadorVM {
    int tabela_paginas[TOTAL_PAGINAS];
    signed char ram[TOTAL_FRAMES * TAMANHO_PAGINA];
    int proximo_frame;

    int pagina_do_frame[TOTAL_FRAMES];


    Elo lista[TOTAL_FRAMES];
    int primeiro;                        /* frame mais novo da lista */
    int ultimo;
    int frames_ocupados;

    int total_enderecos;
    int total_page_faults;
}GerenciadorVM;


void inicializar_vm(GerenciadorVM *vm);


Endereco quebrar_endereco(int endereco_logico);


int obter_frame(GerenciadorVM *vm, int pagina, int move_no_acerto, FILE *disco, int *pagina_removida);

signed char ler_byte_ram(GerenciadorVM *vm, int endereco_fisico);

void usar_frame(GerenciadorVM *vm, int frame, int move_no_acerto);
#endif