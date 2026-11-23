#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>

#define TAMANHO_PAGINA 256
#define TOTAL_PAGINAS 256
#define TOTAL_FRAMES 256

struct Endereco {
    int pagina;
    int offset;
};

struct GerenciadorVM {
    int tabela_paginas[TOTAL_PAGINAS];
    char ram[TOTAL_FRAMES * TAMANHO_PAGINA];
    int proximo_frame;
    int total_enderecos;
    int total_page_faults;
};


void inicializar_vm(struct GerenciadorVM *vm);


struct Endereco quebrar_endereco(int endereco_logico);


int obter_frame(struct GerenciadorVM *vm, int pagina, FILE *backing_store);


char ler_byte_ram(struct GerenciadorVM *vm, int endereco_fisico);

#endif