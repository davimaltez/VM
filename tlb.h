#ifndef TLB_H
#define TLB_H

#include "memoria.h"

#define TAMANHO_TLB 16

typedef struct {
    int pagina;
    int frame;
    int valida;
} EntradaTLB;

typedef struct {
    EntradaTLB entradas[TAMANHO_TLB];
    Elo lista[TAMANHO_TLB];
    int primeiro;
    int ultimo;
    int hits;
} TLB;

typedef struct {
    TLB *tlb;
    int posicao;
    int pagina;
    int achou;
} FichaBusca;


void inicializar_tlb(TLB *tlb);
int buscar_na_tlb(TLB *tlb, int pagina);
void usar_entrada_tlb(TLB *tlb, int pos, int move_no_acerto);
void inserir_na_tlb(TLB *tlb, int pagina, int frame);
void invalidar_na_tlb(TLB *tlb, int pagina);
int buscar_na_tlb_threads(TLB *tlb, int pagina);

#endif