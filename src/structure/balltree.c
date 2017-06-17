#include "structure/balltree.h"

balltree_t *create_balltree(set_t *dataset, int k) {
  balltree_t *balltree = (balltree_t *) calloc(1, sizeof(balltree_t));
  balltree->nodes_size = 0;
  balltree->node_cnt = 0;

  balltree->dataset = dataset;
  balltree->k = k;
  balltree->root = build_tree(balltree, dataset, k);

  balltree->radi_size = balltree->node_cnt;

  balltree->radi = (double *) malloc(sizeof(double) * balltree->radi_size);
  balltree->nodes_info = (int *) malloc(sizeof(int) * balltree->nodes_size);
  balltree->centers = create_set(balltree->node_cnt);
  
  balltree->nodes_info[0] = balltree->node_cnt;
  balltree->node_cnt = 1;
  pack_nodes(balltree, balltree->root, -1, -1);

  return balltree;
}

balltree_t *get_balltree(set_t *dataset, set_t *centers, double *radi, int *packed, int k) {
  balltree_t *balltree = (balltree_t *) calloc(1, sizeof(balltree_t));

  balltree->dataset = dataset;
  balltree->centers = centers;
  balltree->radi = radi;
  balltree->k = k;

  unpack_nodes(balltree, packed);

  return balltree;
}

void delete_nodes(node_t *n) {
  if (n == NULL)
    return;

  delete_nodes(n->left);
  delete_nodes(n->right);

  free(n->points);
  free(n->center->value);
  free(n->center);
  free(n);
}

void delete_balltree(balltree_t *balltree) {
  delete_nodes(balltree->root); 

  for (int i = 0; i < balltree->dataset->size; ++i) {
    free(balltree->dataset->data[i]->value);
    free(balltree->dataset->data[i]);
  }

  free(balltree->dataset->data);
  free(balltree->dataset);
}

void pack_nodes(balltree_t *bt, node_t *n, int par, int dir) {
  if (n == NULL)
    return;

  //bt->centers->data[n->id] = n->center;
  bt->centers->data[n->id] = create_point(n->center->size, -1);
  for (int i = 0; i < n->center->size; ++i)
    bt->centers->data[n->id]->value[i] = n->center->value[i];
  bt->radi[n->id] = n->radius;

  bt->nodes_info[bt->node_cnt++] = n->size;
  bt->nodes_info[bt->node_cnt++] = n->id;
  bt->nodes_info[bt->node_cnt++] = n->leaf;
  //bt->nodes_info[bt->node_cnt++] = (n->left != NULL) ? n->left->id : -1;
  //bt->nodes_info[bt->node_cnt++] = (n->right != NULL) ? n->right->id : -1;

  bt->nodes_info[bt->node_cnt++] = par;
  bt->nodes_info[bt->node_cnt++] = dir; 

  if (n->size > 5)
    for (int i = 0; i < n->points->size; ++i)
      bt->nodes_info[bt->node_cnt++] = n->points->data[i]->id;

  pack_nodes(bt, n->left, n->id, 0);
  pack_nodes(bt, n->right, n->id, 1);
}

void unpack_nodes(balltree_t *bt, int *packed) {
  int k = 0;
  int cnt = packed[k++];
  node_t **nodes = (node_t **) malloc(sizeof(node_t *) * cnt);

  for (int i = 0; i < cnt; ++i) {
    int size = packed[k++];
    int id = packed[k++];
    int leaf = packed[k++];

    int par = packed[k++];
    int dir = packed[k++];

    node_t *node = (node_t *) calloc(1, sizeof(node_t));
    node->size = size;
    node->id = id;
    node->leaf = leaf;
    node->radius = bt->radi[id];
    node->left = node->right = NULL;

    node->center = bt->centers->data[id];

    if (size > 5) {
      node->points = create_set(size - 5);
      for (int j = 0; j < size - 5; ++j)
        node->points->data[j] = bt->dataset->data[packed[k++]];
    }

    if (par == -1)
      bt->root = node;
    else if (!dir)
      nodes[par]->left = node;
    else
      nodes[par]->right = node;

    nodes[id] = node;
  }

  free(nodes);
}

node_t *build_tree(balltree_t *bt, set_t *points, int k) {
  int size = 5;
  node_t *node = (node_t *) calloc(1, sizeof(node_t));
  node->id = bt->node_cnt++;

  calc_center(points, &(node->center));
  
  if (points->size <= k) {
    size += points->size;
    node->points = points;

    node->radius = 0.0;
    node->leaf = TRUE;
    node->left = node->right = NULL;
  } else {
    int left_idx = 0;
    set_t *left_part;
    set_t *right_part;

    node->radius = calc_radius(node->center, points, &left_idx);
    partition(points, &left_part, &right_part, left_idx);

    node->leaf = FALSE;

    node->left = build_tree(bt, left_part, k);
    node->right = build_tree(bt, right_part, k);
  }

  node->size = size;
  bt->nodes_size += size;

  return node;
}

int search(balltree_t *bt, const point_t *point, int *result) {
  priority_queue_t pq;
  pq_init(&pq);

  recursive_search(bt, bt->root, point, &pq);
  
  pq_to_array(&pq, result);

  for (int i = 0; i < pq.size; ++i)
    result[i] = bt->dataset->data[result[i]]->mclass;

  pq_delete(&pq);
  return pq.size;
}

void recursive_search(balltree_t *bt, node_t *node, const point_t *point, 
    priority_queue_t *pq) {

  tuple_t top = pq_first(pq);

  if (node->leaf) {
    for (int i = 0; i < node->points->size; ++i) {
      double dist = distance(point, node->points->data[i]);
    
      if (pq->size == 0 || dist < distance(point, bt->dataset->data[top.y])) {
        
        // Adiciona distancia e indice do ponto a fila de prioridades
        tuple_t entry = {
          .x = dist,
          .y = node->points->data[i]->id
        };

        pq_insert(pq, entry);

        // Remove ultimo elemento da fila de prioridades caso o tamanho tenha
        // ultrapassado k
        if (pq->size > bt->k)
          pq_remove_first(pq); 
      }
    }
  } else {
    double dist_left = distance(point, node->left->center);
    double dist_right = distance(point, node->right->center);

    // Condicao para decidir qual subarvore deve ser buscada primeiro
    if (dist_left <= dist_right) {

      // Busca subarvore da esquerda somente se houver a possibilidade
      // de ter um ponto mais proximo que a maior distancia calculada
      // na particao da esquerda.
      if (pq->size == 0 || (dist_left <= top.x + node->left->radius))
        recursive_search(bt, node->left, point, pq);

      // A mesma condicao eh verificada para a direita
      if (pq->size == 0 || (dist_right <= top.x + node->right->radius))
        recursive_search(bt, node->right, point, pq);
    } else {

      // O mesmo eh feito na ordem oposta caso a distancia do ponto ate
      // o centro da direita seja menor que ao da esquerda
      if (pq->size == 0 || (dist_right <= top.x + node->right->radius))
        recursive_search(bt, node->right, point, pq);

      if (pq->size == 0 || (dist_left <= top.x + node->left->radius))
        recursive_search(bt, node->left, point, pq);
    }
  }
}

void partition(set_t *points, set_t **left, set_t **right, int left_ind) {
  int right_ind = 0;
  double dist, grt_dist = -1.0;
  double left_dist, right_dist;

  //Ponto que sera usado para obter a particao da direita
  point_t *rm_point;
  
  //Ponto que sera usado para obter a particao da esquerda
  point_t *lm_point = points->data[left_ind];

  // Encontra o ponto mais distante do ponto da esquerda para ser o ponto
  // da direita
  for (int i = 0; i < points->size; ++i) {
    if (i != left_ind) {
      dist = distance(lm_point, points->data[i]);

      if (dist > grt_dist) {
        grt_dist = dist;
        right_ind = i;
      }
    }
  }

  rm_point = points->data[right_ind];

  int ri = 0, li = 0;

  int *left_idxs  = (int *) calloc(1, sizeof(int) * points->size);
  int *right_idxs = (int *) calloc(1, sizeof(int) * points->size);

  left_idxs[li++] = left_ind;
  right_idxs[ri++] = right_ind;

  // Conjunto de indices dos pontos que serao usados na particao da esquerda
  // e os que serao usados na particao da direita
  for (int i = 0; i < points->size; ++i) {
    if (i != left_ind && i != right_ind) {
      left_dist = distance(lm_point, points->data[i]);
      right_dist = distance(rm_point, points->data[i]);

      if (left_dist < right_dist)
        left_idxs[li++] = i;
      else
        right_idxs[ri++] = i;
    }
  }

  *left = create_set(li);
  *right = create_set(ri);

  for (int i = 0; i < li; ++i)
    (*left)->data[i] = points->data[left_idxs[i]];

  for (int i = 0; i < ri; ++i)
    (*right)->data[i] = points->data[right_idxs[i]];

  free(left_idxs);
  free(right_idxs);
}

void calc_center(set_t *points, point_t **center) {
  int n_dim = points->data[0]->size;
  *center = create_point(n_dim, -1);

  // Calcula media de todos os pontos
  for (int i = 0; i < points->size; ++i)
    for (int j = 0; j < n_dim; ++j)
      (*center)->value[j] += points->data[i]->value[j];
  
  double div = 1.0 / ((double) points->size);

  for (int i = 0; i < n_dim; ++i)
    (*center)->value[i] *= div;
}

double calc_radius(point_t *center, set_t *points, int *index) {
  *index = 0;

  double dist = distance(center, points->data[0]);
  double radius = dist;

  // Encontra o ponto mais distante de centro, a distancia sera usada como
  // raio da particao
  for (int i = 1; i < points->size; ++i) {
    dist = distance(center, points->data[i]);

    if (dist > radius) {
      radius = dist;
      *index = i;
    }
  }

  return radius;
}
