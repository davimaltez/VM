# Implementação 6 - Gerenciador de Memória Virtual

Davi Maltez Costa Vasconcelos - Infraestrutura de Software

Implementação do "Designing a Virtual Memory Manager" (Silberschatz, 10ª edição, p. P-51) com as modificações do enunciado: memória física de 128 frames, TLB de 16 entradas, substituição FIFO e LRU para páginas e para a TLB, e busca na TLB com uma thread por entrada.

## Compilação

```
make
```

Gera o executável `vm` (versão com TLB e threads).

Outras versões, usadas para testar os fluxos da rubrica:

| Comando | Versão gerada |
| --- | --- |
| `make` | TLB com uma thread por entrada, 128 frames |
| `make sem_thread` | TLB com busca sequencial, 128 frames |
| `make sem_tlb` | sem TLB, 128 frames |
| `make sem_tlb_256` | sem TLB, 256 frames |
| `make clean` | apaga o executável |

Todas geram o mesmo executável `vm`, então rode `make` de novo para voltar à versão de entrega.

## Execução

```
./vm <arquivo_de_enderecos> <algoritmo_pagina> <algoritmo_tlb>
```

- `arquivo_de_enderecos`: um endereço lógico (inteiro) por linha, como o `addresses.txt`
- `algoritmo_pagina`: `fifo` ou `lru`
- `algoritmo_tlb`: `fifo` ou `lru`

Exemplo:

```
./vm addresses.txt lru fifo
```

O `BACKING_STORE.bin` precisa estar na mesma pasta.

## Saída

- `correct.txt`: uma linha por endereço, no formato `Virtual address: X Physical address: Y Value: Z`
- No terminal: total de endereços, page faults e TLB hits

## Tratamento de erros

O programa mostra uma mensagem e encerra quando:

- a quantidade de argumentos não é 3
- o algoritmo de página ou da TLB não é `fifo` ou `lru` (em minúsculo)
- o arquivo de endereços não existe, não abre ou está vazio
- alguma linha está em branco ou tem algo que não seja um inteiro positivo
- o `BACKING_STORE.bin` não existe ou não pode ser lido

## Arquivos

| Arquivo | O que faz |
| --- | --- |
| `main.c` | lê os endereços, consulta a TLB, cai na tabela de páginas no miss e escreve o `correct.txt` |
| `validacao.c/.h` | valida argumentos e arquivos |
| `memoria.c/.h` | tabela de páginas, RAM, leitura do backing store e lista FIFO/LRU dos frames |
| `tlb.c/.h` | TLB de 16 entradas: busca (sequencial ou com threads), inserção e invalidação |
| `politica.c/.h` | tabela com as políticas aceitas (`fifo` e `lru`) |
| `Makefile` | compilação e limpeza |

## Decisões de projeto

- **FIFO e LRU com uma lista duplamente encadeada** (feita com índices, sem malloc). A página nova entra no início e a vítima é sempre o fim. No LRU, uma página usada volta para o início; no FIFO, não. A mesma ideia é usada nos frames e na TLB.
- **Invalidação na TLB:** quando uma página sai da RAM, o `obter_frame` informa qual foi e a `main` tira essa página da TLB, para não sobrar tradução apontando para um frame antigo.
- **Acerto na TLB também atualiza o LRU da RAM**, porque a página foi usada mesmo sem passar pela tabela de páginas.
- **Vaga livre na TLB é usada antes de expulsar alguém.**
- **Tabela de políticas:** os algoritmos aceitos ficam num lugar só, usado tanto na validação quanto na escolha do algoritmo.
- **Threads sem mutex:** cada thread olha uma entrada da TLB e escreve o resultado só na sua própria struct. A `main` só lê depois do `pthread_join`, então nenhuma thread disputa a mesma variável.
- **A TLB decide sozinha** se está ligada e se usa threads (definido na compilação). A `main` sempre chama `buscar_na_tlb`.
- **Endereços:** só os 16 bits de baixo são usados (página = 8 bits, offset = 8 bits).