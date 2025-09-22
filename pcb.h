#ifndef PCB_H
#define PCB_H

#include <stdio.h>

// Diferentes Estados para os Ciclos
typedef enum {
    CREATE,
    READY,
    RUN,
    FINISH,
    BLOCK,
    SUS_BLOCK,
    SUS_READY
} States;

// Estrutura do Bloco de Controle de Processo (PCB).
// Armazena todas as informações relevantes de um processo.
typedef struct PCB {
    int id;
    States state;
    int remaining_time;
    int mem_needed;
    int block_moment;
    int block_time;
    int quantum_used;
    struct PCB *next;
} PCB;

// Funções para manipulação de filas de processo, para manipulação de PCB
PCB* pcb_new();     // Aloca e inicializa um novo PCB
void pcb_push(PCB **queue, PCB *item, int t);   // Adiciona um PCB ao final de uma fila
PCB* pcb_pop(PCB **queue);  // Remove e retorna o primeiro PCB de uma fila

// Funções de log
const char* state_to_string(States s);      // Converte um valor do enum `States` para uma string descritiva
void pcb_log(PCB *p, int time, int processor_id);   // Imprime no console o estado atual de um processo

#endif
