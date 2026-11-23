#include <stdio.h>
#include <stdlib.h>
#include "validacao.h"
#include <string.h>
#include "memoria.h"

int main(int argc, char *argv[]) {

    if (!validar_argumentos(argc, argv)) {
        return 1;
    }

    char *arquivo_enderecos = argv[1];
    char *algoritmo_pagina = argv[2];
    char *algoritmo_tlb = argv[3];

    if (!validar_arquivo_enderecos(arquivo_enderecos)) {
        return 1;
    }

    if (!validar_arquivo_backing_store("BACKING_STORE.bin")) {
        return 1;
    }

    FILE *arquivo_entrada = fopen(arquivo_enderecos, "r");
    if (arquivo_entrada == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo %s\n", arquivo_enderecos);
        return 1;
    }

    FILE *arquivo_backing_store = fopen("BACKING_STORE.bin", "rb");
    if (arquivo_backing_store == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o BACKING_STORE.bin\n");
        fclose(arquivo_entrada);
        return 1;
    }

    FILE *arquivo_saida = fopen("correct.txt", "w");
    if (arquivo_saida == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel criar o arquivo correct.txt\n");
        fclose(arquivo_entrada);
        fclose(arquivo_backing_store);
        return 1;
    }

    GerenciadorVM vm;
    inicializar_vm(&vm);

    int endereco_virtual = 0;
    while (fscanf(arquivo_entrada, "%d", &endereco_virtual) == 1) {
        vm.total_enderecos++;

        Endereco endereco = quebrar_endereco(endereco_virtual);

        int move_no_acerto = (strcmp(algoritmo_pagina, "lru") == 0);
        int frame = obter_frame(&vm, endereco.pagina, move_no_acerto, arquivo_backing_store);

            if (frame == -1) {
                fclose(arquivo_entrada);
                fclose(arquivo_backing_store);
                fclose(arquivo_saida);
                return 1;
            }

        int endereco_fisico = (frame * TAMANHO_PAGINA) + endereco.offset;

        signed char valor_lido = ler_byte_ram(&vm, endereco_fisico);

        fprintf(arquivo_saida, "Virtual address: %d Physical address: %d Value: %d\n",
                endereco_virtual, endereco_fisico, valor_lido);
    }

    printf("Total de enderecos: %d | Page faults: %d\n", vm.total_enderecos, vm.total_page_faults);

    fclose(arquivo_entrada);
    fclose(arquivo_backing_store);
    fclose(arquivo_saida);

    return 0;
}