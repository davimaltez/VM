#include <stdio.h>
#include <stdlib.h>
#include "validacao.h"
#include <string.h>
#include "memoria.h"
#include "tlb.h"
#include "politica.h"

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

    TLB tlb;
    inicializar_tlb(&tlb);

    int move_no_acerto = achar_politica(algoritmo_pagina);
    int tlb_move_no_acerto = achar_politica(algoritmo_tlb);

    int endereco_virtual = 0;
    while (fscanf(arquivo_entrada, "%d", &endereco_virtual) == 1) {
        vm.total_enderecos++;

        Endereco endereco = quebrar_endereco(endereco_virtual);

        int frame;
        #if defined(SEM_TLB)
                int pos_tlb = -1;
        #elif defined(TLB_THREADS)
                int pos_tlb = buscar_na_tlb_threads(&tlb, endereco.pagina);
        #else
                int pos_tlb = buscar_na_tlb(&tlb, endereco.pagina);
        #endif

        if (pos_tlb != -1) {
            tlb.hits++;
            frame = tlb.entradas[pos_tlb].frame;
            usar_entrada_tlb(&tlb, pos_tlb, tlb_move_no_acerto);
            usar_frame(&vm, frame, move_no_acerto);
        } else {
            int pagina_removida;
            frame = obter_frame(&vm, endereco.pagina, move_no_acerto,
                                arquivo_backing_store, &pagina_removida);

            if (frame == -1) {
                fclose(arquivo_entrada);
                fclose(arquivo_backing_store);
                fclose(arquivo_saida);
                return 1;
            }

            if (pagina_removida != -1) {
                invalidar_na_tlb(&tlb, pagina_removida);
            }

            inserir_na_tlb(&tlb, endereco.pagina, frame);
        }

        int endereco_fisico = (frame * TAMANHO_PAGINA) + endereco.offset;

        signed char valor_lido = ler_byte_ram(&vm, endereco_fisico);

        fprintf(arquivo_saida, "Virtual address: %d Physical address: %d Value: %d\n",
                endereco_virtual, endereco_fisico, valor_lido);
    }

       printf("Total de enderecos: %d | Page faults: %d | TLB hits: %d\n",
           vm.total_enderecos, vm.total_page_faults, tlb.hits);

    fclose(arquivo_entrada);
    fclose(arquivo_backing_store);
    fclose(arquivo_saida);

    return 0;
}