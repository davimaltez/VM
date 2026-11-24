#ifndef POLITICA_H
#define POLITICA_H

typedef struct {
    char *nome;
    int move_no_acerto;
} Politica;

int achar_politica(char *nome);

#endif