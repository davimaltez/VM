#include <stdio.h>
#include "memoria.h"

void inicializar_vm(struct GerenciadorVM *vm) {

    for (int i = 0; i < TOTAL_PAGINAS; i++) {
        vm->tabela_paginas[i] = -1;
    }

    vm->proximo_frame = 0;
    vm->total_enderecos = 0;
    vm->total_page_faults = 0;
    
}

struct Endereco quebrar_endereco(int endereco_logico) {
    struct Endereco endereco;

    // Pega apenas os 16 bits inferiores (0 a 65535)
    int endereco_16bits = endereco_logico % 65536;

    endereco.pagina = endereco_16bits / TAMANHO_PAGINA;
    endereco.offset = endereco_16bits % TAMANHO_PAGINA;

    return endereco;
}

int obter_frame(struct GerenciadorVM *vm, int pagina, FILE *arquivo_backing_store) {

    if (vm->tabela_paginas[pagina] != -1) {
        return vm->tabela_paginas[pagina];
    }

    vm->total_page_faults++;

    int frame_escolhido = vm->proximo_frame;

    vm->proximo_frame++;

    char buffer[TAMANHO_PAGINA];

    int posicao_no_disco = pagina * TAMANHO_PAGINA;

    fseek(arquivo_backing_store, posicao_no_disco, SEEK_SET);

    fread(buffer, 1, TAMANHO_PAGINA, arquivo_backing_store);

    // Copia os 256 bytes lidos para o frame escolhido na RAM
    int inicio_frame = frame_escolhido * TAMANHO_PAGINA;

    for (int i = 0; i < TAMANHO_PAGINA; i++) {
        vm->ram[inicio_frame + i] = buffer[i];
    }

    vm->tabela_paginas[pagina] = frame_escolhido;

    return frame_escolhido;
}

char ler_byte_ram(struct GerenciadorVM *vm, int endereco_fisico) {
    return vm->ram[endereco_fisico];
}