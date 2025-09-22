#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcb.h"
#include "escalonador.h"

// ==== Variáveis globais ====
// Filas de processos para cada estado do sistema.
PCB *_create, *_ready, *_running, *_finish, *_blocked;
int _memSize, _quantum, _nprocs, _time, _pcount;// Configurações da simulação.
int processors_count; // Número de processadores (CPU cores).
SchedAlgo _algo;// Algoritmo de escalonamento a ser utilizado.

int main(int argc, char *argv[]) {
    // ==== Seleção do Algoritmo ====
    // Determina o algoritmo de escalonamento com base nos argumentos da linha de comando.
    // O padrão é Round Robin (RR) se nenhum ou um algoritmo inválido for fornecido.
    if (argc > 2) {
        if (strcmp(argv[2], "fifo") == 0) _algo = ALG_FIFO;
        else if (strcmp(argv[2], "sjf") == 0) _algo = ALG_SJF;
        else if (strcmp(argv[2], "rr") == 0) _algo = ALG_RR;
        else if (strcmp(argv[2], "srt") == 0) _algo = ALG_SRT; 
        else {
            printf("Algoritmo inválido! Usando RR.\n");
            _algo = ALG_RR;
        }
    } else {
        _algo = ALG_RR;
    }
    
    // Constrói o nome do algoritmo para a mensagem inicial
    const char* algo_str;
    switch(_algo) {
        case ALG_FIFO: algo_str = "FIFO"; break;
        case ALG_SJF:  algo_str = "SJF"; break;
        case ALG_RR:   algo_str = "RR"; break;
        case ALG_SRT:  algo_str = "SRT"; break;
        default:       algo_str = "UNKNOWN"; break;
    }
    printf("Executando o algoritmo de escalonamento: %s\n", algo_str);


    // ==== Leitura e Processamento da Entrada ====
    char *input = (argc > 1) ? argv[1] : "2 2|2|3|0 5|1 3|2 4";    // Define uma string de entrada padrão caso nenhuma seja incerida
    char *buffer = malloc(strlen(input) + 1);    // Aloca memória para uma cópia da string de entrada para poder modificá-la.
    strcpy(buffer, input);
    for (int i = 0; i < (int)strlen(buffer); i++)    // Substitui '|' por quebras de linha para facilitar a leitura com sscanf.
        if (buffer[i] == '|') buffer[i] = '\n';
    
    char line[256];
    char *buffp = buffer;
    int count;

    // Lê a memória e o quantum
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    sscanf(line, "%d %d", &_memSize, &_quantum);

    // Lê o número de processadores
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    sscanf(line, "%d", &processors_count);

    // Lê número de processos
    sscanf(buffp, "%[^\n]%n", line, &count);
    buffp += count + 1;
    sscanf(line, "%d", &_nprocs);


    // ==== Inicialização ====
    // Zera as filas de processos e contadores da simulação
    _create = _ready = _running = _finish = _blocked = NULL;
    _time = 0;
    _pcount = 0;
    int nFinished = 0;

    // Lê primeiro processo  para incialização do loop
    int nextT = -1; // Informa o tempo de chegada do próximo processo
    if (sscanf(buffp, "%[^\n]%n", line, &count) >= 1) {
        buffp += count + 1;
        if (sscanf(line, "%d", &nextT) != 1) nextT = -1;
    }

    // Cria um array para representar os processadores e o inicializa como vazio (NULL)
    PCB *processors[processors_count];
    memset(processors, 0, sizeof(processors));






    // ==== 1 Loop Principal da Simulação ====
    // O loop continua até que todos os processos tenham sido finalizados
    while (nFinished < _nprocs) {
        // ==== 1.1 Criação de novos processos ====
        // Verifica se chegou o momento de criar um novo processo
        while (nextT > -1 && _time == nextT) {
            PCB *p = pcb_new();
            p->quantum_used = 0; 
            int t_chegada, t_exec;
            int params = sscanf(line, "%d %d %db%d", &t_chegada, &t_exec, &p->block_moment, &p->block_time);
            if (params < 4) p->block_moment = -1; // Não realiza o bloqueio caso não seja específicado
            p->remaining_time = t_exec;
            p->id = _pcount++;
            p->state = CREATE;
            pcb_log(p, _time, -1);  // Realiza o registro do evento
            pcb_push(&_create, p, _time);   // Adiciona na fila de criação
            
            // Lê o próximo processo da entrada
            if (sscanf(buffp, "%[^\n]%n", line, &count) >= 1) {
                buffp += count + 1;
                if (sscanf(line, "%d", &nextT) != 1) nextT = -1;
            } else {
                nextT = -1;
            }
        }


        // ==== 1.2 CREATE → READY ====
        // Move todos os processos da fila de criação para a fila de prontos
        PCB *p;
        int new_process_arrived = 0; // Flag para indicar se novos processos chegaram
        while ((p = pcb_pop(&_create)) != NULL) {
            new_process_arrived = 1;
            p->state = READY;
            pcb_push(&_ready, p, _time);
            pcb_log(p, _time, -1);
        }


        // ==== 1.3 LÓGICA DE PREEMPÇÃO DO (SRT) ====
        // Se for SRT e um novo processo chegou, verifica se ele é mais curto que algum em execução.
        if (_algo == ALG_SRT && new_process_arrived && _ready != NULL) {
            PCB *shortest_in_ready = _ready;    // Encontra o processo mais curto na fila de prontos, apenas para consulta
            PCB *current = _ready->next;
            while (current != NULL) {
                if (current->remaining_time < shortest_in_ready->remaining_time) {
                    shortest_in_ready = current;
                }
                current = current->next;
            }
            
            // Compara o mais curto da fila de prontos com cada processo em execução
            for (int i = 0; i < processors_count; i++) {
                PCB *running_p = processors[i];
                if (running_p != NULL && running_p->remaining_time > shortest_in_ready->remaining_time) {   // Se um processo está executando e seu tempo restante é maior que o do novo processo mais curto
                    // OCORRE A PREEMPÇÃO! O processo em execução volta para a fila de prontos
                    running_p->state = SUS_READY;
                    pcb_push(&_ready, running_p, _time);
                    pcb_log(running_p, _time, -1);
                    processors[i] = NULL; // Libera o processador
                }
            }
        }


        // ==== 1.4 Despacho para processadores livres ====
        // Atribui processos da fila de prontos para processadores que estão ociosos.
        for (int i = 0; i < processors_count; i++) {
            if (processors[i] == NULL && _ready != NULL) {
                PCB *p = schedule(_algo, &_ready, _quantum, _time);     // Chama a função de escalonamento para decidir qual processo executar
                if (p) {
                    p->state = RUN;
                    processors[i] = p;      // "Coloca" o processo na CPU
                    pcb_log(p, _time, i+1);
                }
            }
        }

        // ==== 1.5 Execução dos processos ====
        // Simula a passagem de um "tick" de tempo para cada processo em execução
        for (int i = 0; i < processors_count; i++) {
            PCB *p = processors[i];
            if (!p) continue;   // Pula se não há processo neste processador
            
            if (p->remaining_time <= 0) {   // Verifica se o processo terminou como resultado do tick ANTERIOR
                p->remaining_time = 0;
                p->state = FINISH;
                pcb_log(p, _time, i+1);
                pcb_push(&_finish, p, _time);   // Move para a fila de finalizados
                processors[i] = NULL;  // Libera o processador
                p->quantum_used = 0;  // Zera o contador de quantum
                continue;
            }
            
            if (p->block_moment >= 0 && _time == p->block_moment) {     // Verifica se é hora de bloquear o processo

                p->state = BLOCK;
                pcb_push(&_blocked, p, _time);
                pcb_log(p, _time, i+1);
                processors[i] = NULL;   // Libera o processador
                p->quantum_used = 0;    // Zera o contador de quantum
                continue;
            }

            p->remaining_time--;    // Decrementa o tempo restante de execução
            p->quantum_used++;

            
            if (_algo == ALG_RR && p->quantum_used >= _quantum) {// Para Round-Robin, verifica se o quantum expirou
                p->state = SUS_READY;
                pcb_push(&_ready, p, _time);    // Devolve o processo para a fila de prontos
                pcb_log(p, _time, -1);
                processors[i] = NULL;   // Libera o processador
                p->quantum_used = 0;    // Zera o contador de quantum
            }
        }

        // ==== 1.6. Desbloqueio de processos ====
        // Percorre a fila de bloqueados para verificar se algum processo pode ser desbloqueado.
        PCB *prev = NULL;
        PCB *curr = _blocked;
        while (curr) {
            PCB *next = curr->next;
            if (_time >= curr->block_moment + curr->block_time) {   // Se o tempo de bloqueio já passou....
                if (prev) prev->next = curr->next;  // Realiza a remoção da Lista
                else _blocked = curr->next;
                curr->next = NULL;
                curr->state = SUS_READY;
                pcb_push(&_ready, curr, _time); // Move para a fila de prontos
                pcb_log(curr, _time, -1);
            } else {
                prev = curr;
            }
            curr = next;
        }
        
        // Limap a fila de processos finalizados e atualiza o contador
        while ((p = pcb_pop(&_finish)) != NULL) {
            nFinished++;
            free(p);    // Libera a memória alocada para 
        // ==== 1.7 Finalização dos Processos ====o PCB
        }
        _time++;    // Avança o tempo da simulação
    }

    free(buffer);
    return 0;
}

//gcc -std=c11 -Wall -O2 -o simso main.c pcb.c escalonador.c
//<mem_size> <quantum> | <Nprocessadores> | <Nprocessos> | <t0> <texec> [b<tbloq>] | <t1> <texec> [b<tbloq>] | ...
// Exemplo de execução:
//./simso "2 5|1|3|0 5 1b3|0 5 4b10|0 6" algoritmo

//Leitura do acima: 
// Memória de 2 unidades, quantum de 5 unidades de tempo
// 1 processador
// 3 processos:
// Processo 0: chega no tempo 0, precisa de 5 unidades de tempo para executar, bloqueia após 1 unidade de tempo por 3 unidades de tempo
// Processo 1: chega no tempo 0, precisa de 5 unidades de tempo para
// Processo 2: chega no tempo 0, precisa de 6 unidades de tempo para executar, não bloqueia
// Algoritmo de escalonamento: fifo, sjf, rr, srt (padrão: rr)