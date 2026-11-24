#include <string.h>
#include "politica.h"

Politica politicas[] = {
    {"fifo", 0},
    {"lru", 1}
};

int total_politicas = sizeof(politicas) / sizeof(politicas[0]);

int achar_politica(char *nome) {
    for (int i = 0; i < total_politicas; i++) {
        if (strcmp(politicas[i].nome, nome) == 0) {
            return politicas[i].move_no_acerto;
        }
    }

    return -1;
}