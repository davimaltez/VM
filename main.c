#include <stdio.h>
#include <stdlib.h>
#include "validacao.h"

#include <stdio.h>
#include "validacao.h"

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

    printf("Argumentos e arquivos validados\n");

    return 0;
}