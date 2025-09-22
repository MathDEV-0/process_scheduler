#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"

// Representação dos estados
const char* states[] = {
    "created",
    "ready",
    "executing",
    "finished",
    "blocked",
    "suspended, blocked",
    "suspended, ready"
};

PCB* pcb_new() {    //Aloca memória e inicializa um novo PCB com valores padrão
    PCB* p = malloc(sizeof(PCB));
    if (!p) return NULL; // Ponteiro para o novo PCB criado, ou NULL se a alocação falhar
    // Inicializa os campos com valores padrão
    p->id = -1;
    p->state = CREATE;
    p->next = NULL;
    p->remaining_time = 0;
    p->block_moment = -1;   // -1 indica que não há bloqueio programado
    p->block_time = 0;
    return p; 
}

/**
 * @param queue Ponteiro para o ponteiro do início da fila
 * @param item O PCB a ser adicionado
 * @param t O tempo atual
 */
void pcb_push(PCB **queue, PCB *item, int t) {//Adiciona um item PCB no final de uma fila, implementada como lista ligada
    if (!item) return;
    item->next = NULL; // Garante que o novo item será o último da fila
    if (*queue == NULL) {
        // Se a fila está vazia, o novo item se torna o primeiro
        *queue = item;
    } else {
        // Se não, percorre a fila até o final e insere o item
        PCB *tmp = *queue;
        while (tmp->next != NULL) tmp = tmp->next;
        tmp->next = item;
    }
}

/**
 * @param queue Ponteiro para o ponteiro do início da fila
 */
PCB* pcb_pop(PCB **queue) { // Remove e retorna o primeiro item de uma fila
    if (*queue != NULL) {
        PCB* aux = *queue;    // Salva o ponteiro para o primeiro item
        *queue = aux->next;   // Atualiza o início da fila para o segundo item
        aux->next = NULL;     // Desconecta o item removido do resto da fila
        return aux;     //PCB* O item que foi removido
    }
    return NULL; //NULL se a fila estava vazia
}

/**
 * @param s O estado do processo.
 * @return const char* A string correspondente ao estado.
 */
const char* state_to_string(States s) {//Converte um valor do enum `States` para sua representação em string
    // Usa um switch para retornar a string correta com base no valor do enum
    switch(s) {
        case CREATE:     return "created";
        case READY:      return "ready";
        case RUN:        return "executing";
        case FINISH:     return "finished";
        case BLOCK:      return "blocked";
        case SUS_BLOCK:  return "suspended, blocked";
        case SUS_READY:  return "suspended, ready";
        default:         return "unknown";
    }
}

/**
 * @param p O processo sobre o qual o log será gerado.
 * @param time O tempo atual da simulação.
 * @param processor_id O ID do processador onde o evento ocorreu (-1 se não estiver em um processador).
 */
void pcb_log(PCB *p, int time, int processor_id) { // Imprime uma mensagem de log formatada sobre o estado de um processo
    if (!p) return;
    // Formata a saída de forma diferente se o processo está associado a um processador.
    if (processor_id >= 0)
        printf("%02d:P%d -> %s (%d) %d\n", time, p->id, state_to_string(p->state), p->remaining_time, processor_id);
    else
        printf("%02d:P%d -> %s (%d)\n", time, p->id, state_to_string(p->state), p->remaining_time);
}
