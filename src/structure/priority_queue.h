#ifndef _PRIORITY_QUEUE_H
#define _PRIORITY_QUEUE_H

#include <stdio.h>

#include "structure/avl.h"

typedef struct priority_queue_t {
  int size;
  avl_t tree; 
} priority_queue_t;


// Inicia fila de prioridades
void pq_init(priority_queue_t *pq);

// Remove fila de prioridades da memoria 
void pq_delete(priority_queue_t *pq);

// Insere elemento
void pq_insert(priority_queue_t *pq, tuple_t key);

// Retorna primeiro elemento da fila
tuple_t pq_first(priority_queue_t *pq);

// Remove maior elemento da fila
void pq_remove_first(priority_queue_t *pq);

// Tranforma fila de prioridades em uma array de inteiros simples
void pq_to_array(priority_queue_t *pq, int *array);

#endif
