#include <stdio.h>
#include <stdlib.h>
#include "escalonador.h"

/**
*   Função principal de escalonamento que realiza o envio
*   O qual, chama o algoritmo de escalonamento correto com base na configuração da simulação
*   @param algo O algoritmo a ser usado (FIFO, SJF, RR, SRT).
*   @param ready Ponteiro para a fila de processos prontos.
*   @param quantum Valor do quantum (usado apenas pelo RR).
*   @param t Tempo atual da simulação.
*   @return PCB retorna o processo escolhido para executar, ou NULL se a fila estiver vazia.
*/
PCB* schedule(SchedAlgo algo, PCB **ready, int quantum,int t) {
    switch(algo) {    // Usa um switch para chamar a função do algoritmo de escalonamento apropriado.
        case ALG_FIFO: return fifo_sch(ready);
        case ALG_SJF:  return sjf_sch(ready);
        case ALG_RR:   return rr_sch(ready, quantum,t);
        case ALG_SRT:  return srt_sch(ready); 
        default: return NULL;
    }
}

// Algoritmo FIFO (First-In, First-Out), o qual retorna o primeiro processo da fila de prontos, sem nenhum critério de seleção
PCB* fifo_sch(PCB **ready) { // O parâmetro (ready) refresenta ponteiro para fila de processos prontos
    return pcb_pop(ready); //   Retorna o primeiro processo da fila
}

//Algoritmo SJF (Shortest Job First) não-preemptivo, o qual procura na fila de prontos pelo processo com o menor tempo de execução restante
PCB* sjf_sch(PCB **ready){ // O parâmetro (ready) refresenta ponteiro para fila de processos prontos
    if(*ready == NULL) return NULL; // Retorna nulo se a fila estiver vazia
    // Ponteiros para percorrer a lista e encontrar o processo mais curto
    PCB *prev = NULL;
    PCB *curr = *ready;
    PCB *shortest = curr; // Assume que o primeiro é o mais curto inicialmente
    PCB *shortest_prev = NULL; // Ponteiro para o nó anterior ao mais curto

    
    while(curr != NULL) {   // Itera por toda a fila de prontos
        if(curr->remaining_time < shortest->remaining_time) {// Se encontrar um processo com tempo restante menor, atualiza 'shortest'
            shortest = curr;
            shortest_prev = prev;
        }
        prev = curr;
        curr = curr->next;
    }

    // Remove o processo "shortest" da fila de prontos, ajustando os ponteiros
    if(shortest_prev == NULL) {
        *ready = shortest->next; // O processo mais curto era o primeiro da fila
    } else {
        shortest_prev->next = shortest->next; // O mais curto estava no meio ou fim
    }

    shortest->next = NULL;  // Isola o processo removido da lista

    return shortest;
}

/**
    Algoritmo SRT (Shortest Remaining Time), o qual em termos de seleção do próximo processo, 
    a lógica é idêntica à do SJF, escolher o processo com o menor tempo restante. 
    A principal característica do SRT, é que a preempção, é tratada no loop principal no Main.c
 */
PCB* srt_sch(PCB **ready) { // O parâmetro (ready) refresenta ponteiro para fila de processos prontos
    // A lógica para encontrar o trabalho com menor tempo restante é a mesma do SJF.
    return sjf_sch(ready);
}

/**
    Algoritmo Round Robin (RR)
    A função de escalonamento do RR se comporta como o FIFO: ela apenas entrega o próximo processo da fila
    A lógica de preempção (retirar do processador após o quantum expirar) é tratada no loop principal no Main.c

 * @param ready Ponteiro para a fila de processos prontos.
 * @param quantum Valor do quantum.
 * @param t Tempo atual da simulação.
 * @return PCB* O primeiro processo da fila.
 */
PCB* rr_sch(PCB **ready, int quantum,int t) {   
    // (ready) refresenta ponteiro para fila de processos prontos
    // (quantum) representa o valor do quantum
    // (t) representa o tempo atual da simulação
    return pcb_pop(ready); // Primeiro processo da fila.
}