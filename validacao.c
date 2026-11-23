#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "validacao.h"

int validar_argumentos(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Erro: Quantidade de argumentos invalida\n");
        return 0;
    }

    // Validação do algoritmo da pagina 
    if (strcmp(argv[2], "fifo") != 0 && strcmp(argv[2], "lru") != 0) {
        fprintf(stderr, "Erro: Algoritmo de pagina invalido, use fifo ou lru\n");
        return 0;
    }

    // Validação do algoritmo da TLB 
    if (strcmp(argv[3], "fifo") != 0 && strcmp(argv[3], "lru") != 0) {
        fprintf(stderr, "Erro: Algoritmo de TLB invalido, use fifo ou lru\n");
        return 0;
    }

    return 1;
}

int validar_arquivo_enderecos(const char *nome_arquivo) {

    FILE *arquivo = fopen(nome_arquivo, "r");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo %s\n", nome_arquivo);
        return 0;
    }

    char linha[256];
    int numero_linha = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
            
        numero_linha++;
        int tamanho = strlen(linha);

        // Tira o \n ou \r do final da linha
        while (tamanho > 0 && (linha[tamanho - 1] == '\n' || linha[tamanho - 1] == '\r' || linha[tamanho - 1] == ' '  || linha[tamanho - 1] == '\t')) {
                            
            linha[tamanho - 1] = '\0';
            tamanho--;
            
        }

        //Linha em brancoo
        if (tamanho == 0) {
            fprintf(stderr, "Erro: Linha %d esta vazia no arquivo %s.\n", numero_linha, nome_arquivo);
            fclose(arquivo);
            return 0;
        }

        //Confere se é número
        for (int i = 0; i < tamanho; i++) {
            if (!isdigit(linha[i])) {
                fprintf(stderr, "Erro: Conteudo invalido na linha %d (%s). Esperado apenas inteiros.\n", numero_linha, linha);
                fclose(arquivo);
                return 0;
            }
        }
    }

    fclose(arquivo);

    //Se não leu nenhuma linha, enão o arquivo estava vazio
    if (numero_linha == 0) {
        fprintf(stderr, "Erro: O arquivo %s esta vazio.\n", nome_arquivo);
        return 0;
    }

    return 1;
}

int validar_arquivo_backing_store(const char *nome_arquivo) {

    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo %s.\n", nome_arquivo);
        return 0;
    }

    fclose(arquivo);
    return 1;
}