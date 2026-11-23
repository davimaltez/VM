#include <stdio.h>
#include "memoria.h"

void inicializar_vm(GerenciadorVM *vm) {

    for (int i = 0; i < TOTAL_PAGINAS; i++) {
        vm->tabela_paginas[i] = -1;
    }   

    for (int i = 0; i < TOTAL_FRAMES; i++) {
        vm->pagina_do_frame[i] = -1;
        vm->lista[i].anterior = -1;
        vm->lista[i].proximo = -1;
    }

    vm->primeiro = -1;
    vm->ultimo = -1;

    vm->frames_ocupados = 0;
    vm->total_enderecos = 0;
    vm->total_page_faults = 0;

}

Endereco quebrar_endereco(int endereco_logico) {
    Endereco endereco;

    // Pega apenas os 16 bits inferiores (0 a 65535)
    int endereco_16bits = endereco_logico % 65536;

    endereco.pagina = endereco_16bits / TAMANHO_PAGINA;
    endereco.offset = endereco_16bits % TAMANHO_PAGINA;

    return endereco;
}


void tirar_da_lista(GerenciadorVM *vm, int frame) {
    int antes = vm->lista[frame].anterior;
    int depois = vm->lista[frame].proximo;
    
    if (antes != -1) {
        vm->lista[antes].proximo = depois;
    } else {
        vm->primeiro = depois;      
    }
    
    if (depois != -1) {
        vm->lista[depois].anterior = antes;
    } else {
        vm->ultimo = antes;         
    }
}

///C oloca o frame na frente da lista (o mais novo)
void colocar_no_inicio(GerenciadorVM *vm, int frame) {
    vm->lista[frame].anterior = -1;
    vm->lista[frame].proximo = vm->primeiro;

    if (vm->primeiro != -1) {
        vm->lista[vm->primeiro].anterior = frame;
    }
    vm->primeiro = frame;

    if (vm->ultimo == -1) {       
        vm->ultimo = frame;
    }
}


int obter_frame(GerenciadorVM *vm, int pagina, int move_no_acerto, FILE *disco) {
    int frame = vm->tabela_paginas[pagina];


    if (frame != -1) {
        if (move_no_acerto) {      
            tirar_da_lista(vm, frame);
            colocar_no_inicio(vm, frame);
        }
        return frame;           
    }

    
    vm->total_page_faults++;

    if (vm->frames_ocupados < TOTAL_FRAMES) {

        frame = vm->frames_ocupados;
        vm->frames_ocupados++;
    }
     else {
        frame = vm->ultimo;     
        tirar_da_lista(vm, frame);
        vm->tabela_paginas[vm->pagina_do_frame[frame]] = -1;
    }

    long posicao = (long)pagina * TAMANHO_PAGINA;
    
    if (fseek(disco, posicao, SEEK_SET) != 0 ||
        fread(&vm->ram[frame * TAMANHO_PAGINA], 1, TAMANHO_PAGINA, disco) != TAMANHO_PAGINA) {
        fprintf(stderr, "Nao consegui ler a pagina %d do backing store\n", pagina);
        return -1;
    }

    colocar_no_inicio(vm, frame);
    vm->tabela_paginas[pagina] = frame;
    vm->pagina_do_frame[frame] = pagina;
    return frame;
}

signed char ler_byte_ram(GerenciadorVM *vm, int endereco_fisico) {

    return vm->ram[endereco_fisico];

}