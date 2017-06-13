#ifndef _BALL_TREE_H
#define _BALL_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "core/dataset.h"
#include "util/types.h"
#include "structure/priority_queue.h"

typedef struct node_t {
  set_t *points;
  point_t *center;

  BOOL leaf;
  int size, id;
  double radius;

  struct node_t *left, *right;
} node_t;

typedef struct balltree_t {
  int k;
  int nodes_size;
  int node_cnt;

  node_t *root;
  set_t *dataset;
  set_t *centers;

  double *radi;
  int *nodes_info;
} balltree_t;

// Cria uma ball tree com todos os elementos do dataset
balltree_t *create_balltree(set_t *dataset, int k);

// Preenche vetor result com os pontos mais proximos de point
int search(balltree_t *bt, const point_t *point, int *result);

// Remove todos os nodos da memoria
void delete_nodes(node_t *n);

// Remove ball tree da memoria
void delete_balltree(balltree_t *balltree);

void pack_nodes(balltree_t *bt, node_t *n);

// Constroi ball tree recursivamente
node_t *build_tree(balltree_t *bt, set_t *points, int k);

// Busca na ball tree recursivamente preenchendo pq com os resultados
void recursive_search(balltree_t *bt, node_t *node, const point_t *point, 
    priority_queue_t *pq);

// Particiona conjunto de pontos em dois subconjuntos
void partition(set_t *points, set_t **left, set_t **right, int left_ind);

// Calcula centro aproximado de um conjunto de pontos
void calc_center(set_t *points, point_t **center);

// Calcula raio de um conjunto de pontos (maior distancia ate o centro)
double calc_radius(point_t *center, set_t *points, int *index);

#endif
