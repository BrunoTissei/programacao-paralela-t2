#ifndef _AVL_H
#define _AVL_H

#include <stdio.h>
#include <stdlib.h>

#include "util/types.h"

typedef struct avl_node_t {
  tuple_t key;
  int height;

  struct avl_node_t *left;
  struct avl_node_t *right;
} avl_node_t;

typedef struct avl_t {
  avl_node_t *root;
} avl_t;


// Remove a AVL da memoria
void avl_delete(avl_node_t *node);

// Insere elemento na AVL
avl_node_t *avl_insert(avl_t *avl, avl_node_t *node, tuple_t key);

// Remove o maior elemento da AVL
void avl_remove_greatest(avl_t *avl);

// Transforma o conteudo da arvore um uma array
void avl_to_array(avl_node_t *node, int *array, int *k);

// Retorna maior elemento da arvore
tuple_t avl_get_largest(avl_t *avl);

#endif
