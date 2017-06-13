#include "structure/priority_queue.h"

void pq_init(priority_queue_t *pq) {
  pq->size = 0;
  pq->tree.root = NULL;
}

void pq_delete(priority_queue_t *pq) {
  avl_delete(pq->tree.root);
}

void pq_insert(priority_queue_t *pq, tuple_t key) {
  pq->tree.root = avl_insert(&(pq->tree), pq->tree.root, key);
  pq->size++;
}

tuple_t pq_first(priority_queue_t *pq) {

  // Caso a fila esteja vazia, retorna um elemento zero
  if (pq->size == 0) {
    return (tuple_t) {
      .x = 0,
      .y = 0
    };
  }

  return avl_get_largest(&(pq->tree));
}

void pq_remove_first(priority_queue_t *pq) {
  avl_remove_greatest(&(pq->tree));
  pq->size--;
}

void pq_to_array(priority_queue_t *pq, int *array) {
  int k = 0;
  avl_to_array(pq->tree.root, array, &k);
}
