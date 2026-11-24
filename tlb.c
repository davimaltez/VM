#include "tlb.h"
#include <pthread.h>

void inicializar_tlb(TLB *tlb) {
    for (int i = 0; i < TAMANHO_TLB; i++) {

        tlb->lista[i].anterior = -1;
        tlb->entradas[i].pagina = -1;
        tlb->entradas[i].valida = 0;
        tlb->lista[i].proximo = -1;
        tlb->entradas[i].frame = -1;
    }

    tlb->primeiro = -1;
    tlb->hits = 0;

    #ifdef SEM_TLB
    tlb->ativa = 0;
    #else
        tlb->ativa = 1;
    #endif

    #ifdef TLB_THREADS
        tlb->usa_threads = 1;
    #else
        tlb->usa_threads = 0;
    #endif
    tlb->ultimo = -1;
}

void remover_da_lista_tlb(TLB *tlb, int pos) {
    int ant = tlb->lista[pos].anterior;
    int prox = tlb->lista[pos].proximo;

    if (ant == -1) {
        tlb->primeiro = prox;
    } else {
        tlb->lista[ant].proximo = prox;
    }

    if (prox == -1) {
        tlb->ultimo = ant;
    } else {
        tlb->lista[prox].anterior = ant;
    }

    tlb->lista[pos].anterior = -1;
    tlb->lista[pos].proximo = -1;
}

void colocar_no_inicio_tlb(TLB *tlb, int pos) {
    tlb->lista[pos].anterior = -1;
    tlb->lista[pos].proximo = tlb->primeiro;

    if (tlb->primeiro != -1) {
        tlb->lista[tlb->primeiro].anterior = pos;
    }

    tlb->primeiro = pos;

    if (tlb->ultimo == -1) {
        tlb->ultimo = pos;
    }
}

int buscar_sequencial(TLB *tlb, int pagina) {
    for (int i = 0; i < TAMANHO_TLB; i++) {
        if (tlb->entradas[i].valida == 1 && tlb->entradas[i].pagina == pagina) {
            return i;
        }
    }

    return -1;
}

void usar_entrada_tlb(TLB *tlb, int pos, int move_no_acerto) {
    if (move_no_acerto == 1) {
        remover_da_lista_tlb(tlb, pos);
        colocar_no_inicio_tlb(tlb, pos);
    }
}

void inserir_na_tlb(TLB *tlb, int pagina, int frame) {

    if (tlb->ativa == 0) {
        return;
    }

    int pos = -1;

    for (int i = 0; i < TAMANHO_TLB; i++) {
        if (tlb->entradas[i].valida == 0) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        pos = tlb->ultimo;
        remover_da_lista_tlb(tlb, pos);
    }

    tlb->entradas[pos].pagina = pagina;
    tlb->entradas[pos].frame = frame;
    tlb->entradas[pos].valida = 1;

    colocar_no_inicio_tlb(tlb, pos);
}

void invalidar_na_tlb(TLB *tlb, int pagina) {
    int pos = buscar_sequencial(tlb, pagina);

    if (pos == -1) {
        return;
    }

    remover_da_lista_tlb(tlb, pos);

    tlb->entradas[pos].pagina = -1;
    tlb->entradas[pos].frame = -1;
    tlb->entradas[pos].valida = 0;
}

void *olhar_entrada(void *arg) {

    FichaBusca *ficha = (FichaBusca *) arg;
    EntradaTLB *e = &ficha->tlb->entradas[ficha->posicao];

    if (e->valida == 1 && e->pagina == ficha->pagina) {
        ficha->achou = 1;
    } else {
        ficha->achou = 0;
    }

    return NULL;
}

int buscar_na_tlb_threads(TLB *tlb, int pagina) {
    pthread_t threads[TAMANHO_TLB];
    FichaBusca fichas[TAMANHO_TLB];
    int criou[TAMANHO_TLB];

    for (int i = 0; i < TAMANHO_TLB; i++) {
        fichas[i].tlb = tlb;
        fichas[i].posicao = i;
        fichas[i].pagina = pagina;
        fichas[i].achou = 0;

        criou[i] = (pthread_create(&threads[i], NULL, olhar_entrada, &fichas[i]) == 0);
    }

    for (int i = 0; i < TAMANHO_TLB; i++) {
        if (criou[i]) {
            pthread_join(threads[i], NULL);
        } else {
            olhar_entrada(&fichas[i]);
        }
    }

    for (int i = 0; i < TAMANHO_TLB; i++) {
        if (fichas[i].achou == 1) {
            return i;
        }
    }

    return -1;
}

int buscar_na_tlb(TLB *tlb, int pagina) {
    if (tlb->ativa == 0) {
        return -1;
    }

    if (tlb->usa_threads == 1) {
        return buscar_na_tlb_threads(tlb, pagina);
    }

    return buscar_sequencial(tlb, pagina);
}