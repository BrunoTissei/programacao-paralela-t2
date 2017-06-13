#include "structure/avl.h"

// Retorna a altura de um nodo
static inline int get_height(avl_node_t *node) {
  return (node == NULL) ? 0 : node->height;
}

// Retorna o balanco de um nodo
static inline int get_balance(avl_node_t *node) {
  return (node == NULL) ? 0 : get_height(node->left) - get_height(node->right);
}

void avl_delete(avl_node_t *node) {
  if (node == NULL)
    return;

  avl_delete(node->left);
  avl_delete(node->right);

  free(node);
}

avl_node_t *rotate_right(avl_node_t *node) {
  avl_node_t *aux1 = node->left;
  avl_node_t *aux2 = aux1->right;

  aux1->right = node;
  node->left = aux2;

  node->height = max(get_height(node->left), get_height(node->right)) + 1;
  aux1->height = max(get_height(aux1->left), get_height(aux1->right)) + 1;
  
  return aux1;
}

avl_node_t *rotate_left(avl_node_t *node) {
  avl_node_t *aux1 = node->right;
  avl_node_t *aux2 = aux1->left;

  aux1->left = node;
  node->right = aux2;

  node->height = max(get_height(node->left), get_height(node->right)) + 1;
  aux1->height = max(get_height(aux1->left), get_height(aux1->right)) + 1;
  
  return aux1;
}

avl_node_t *avl_insert(avl_t *avl, avl_node_t *node, tuple_t key) {
  if (node == NULL) {
    
    // Retorna um novo nodo
    avl_node_t *new = (avl_node_t *) malloc(sizeof(avl_node_t));

    new->key = key;
    new->left = NULL;
    new->right = NULL;
    new->height = 1;

    if (avl->root == NULL)
      avl->root = new;

    return new;
  }

  if (key.x < node->key.x)
    node->left  = avl_insert(avl, node->left, key);
  else
    node->right = avl_insert(avl, node->right, key);

  int balance = get_balance(node);
  node->height = max(get_height(node->left), get_height(node->right)) + 1;

  // Rotaciona arvore para se manter balanceada mesmo apos uma insercao
  if (balance > 1 && key.x < node->left->key.x) {
    return rotate_right(node);
  } else if (balance < -1 && key.x > node->right->key.x) {
    return rotate_left(node);
  } else if (balance > 1 && key.x > node->left->key.x) {
    node->left = rotate_left(node->left);
    return rotate_right(node);
  } else if (balance < -1 && key.x < node->right->key.x) {
    node->right = rotate_right(node->right);
    return rotate_left(node);
  }

  return node;
}

void avl_to_array(avl_node_t *node, int *array, int *k) {
  if (node == NULL)
    return;

  // Preenchimento da array in-order
  avl_to_array(node->left, array, k);
  array[(*k)++] = node->key.y;
  avl_to_array(node->right, array, k);
}

avl_node_t *delete(avl_node_t *node) {
  if (node == NULL)
    return NULL;

  // Percorre arvore recursivamente ate o ultimo nodo a direita
  // (o nodo de maior valor da arvore)
  if (node->right != NULL)
    node->right = delete(node->right);
  else {
    avl_node_t *tmp = node->left;

    // Caso o nodo nao possua nenhum filho
    if (tmp == NULL) {
      tmp = node;
      node = NULL;

    // Caso o nodo possua um filho a esquerda
    } else
      *node = *tmp;

    free(tmp);
  }

  if (node == NULL)
    return NULL;

  int balance = get_balance(node);
  node->height = 1 + max(get_height(node->left), get_height(node->right));

  // Rotaciona arvore para se manter balanceada mesmo apos uma remocao
  if (balance > 1 && get_balance(node->left) >= 0) {
    return rotate_right(node);
  } else if (balance > 1 && get_balance(node->left) < 0) {
    node->left = rotate_left(node->left);
    return rotate_right(node);
  } else if (balance < -1 && get_balance(node->right) <= 0) {
    return rotate_left(node);
  } else if (balance < -1 && get_balance(node->right) > 0) {
    node->right = rotate_right(node->right);
    return rotate_left(node);
  }

  return node;
}

void avl_remove_greatest(avl_t *avl) {
  avl->root = delete(avl->root);
}

tuple_t avl_get_largest(avl_t *avl) {
  avl_node_t *node = avl->root;

  // Percorre a arvore iterativamente em busca do ultimo nodo a direita
  // (que possui o maior valor)
  while (node->right != NULL)
    node = node->right;

  return node->key;
} 
