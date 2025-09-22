#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "pcb.h"

// Algorítmos de Escalonamento
typedef enum {
    ALG_FIFO,
    ALG_SJF,
    ALG_RR,
    ALG_SRT,
} SchedAlgo;


// Protótipos das funções de escalonamento
PCB* schedule(SchedAlgo algo, PCB **ready, int quantum, int t); // Função principal que seleciona o processo a ser executado
// Funções específicas de cada algoritmo
PCB* fifo_sch(PCB **ready);
PCB* sjf_sch(PCB **ready);
PCB* rr_sch(PCB **ready, int quantum, int t);
PCB* srt_sch(PCB **ready);


#endif