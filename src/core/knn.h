#ifndef _KNN_H
#define _KNN_H

#include "core/dataset.h"
#include "structure/balltree.h"

typedef struct knn_classifier_t {
  int k;
  balltree_t *balltree;
} knn_classifier_t;


// Retorna um ponteiro para um classificador alocado na memoria
knn_classifier_t *create_classifier(int k);

// Remove o classificador da memoria
void delete_classifier(knn_classifier_t *knn);

// Cria a balltree com todos os valores do tr_set
void fit(knn_classifier_t *knn_clf, set_t *tr_set);

// Retorna a classe de um ponto
int predict(knn_classifier_t *knn_clf, const point_t *point);

#endif
