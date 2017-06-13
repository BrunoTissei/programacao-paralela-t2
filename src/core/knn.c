#include "core/knn.h"

knn_classifier_t *create_classifier(int k) {
  knn_classifier_t *knn = (knn_classifier_t *) malloc(sizeof(knn_classifier_t));
  knn->k = k;

  return knn;
}

void delete_classifier(knn_classifier_t *knn) {
  delete_balltree(knn->balltree); 

  free(knn->balltree);
  free(knn);
}

void fit(knn_classifier_t *knn_clf, set_t *tr_set) {
  knn_clf->balltree = create_balltree(tr_set, knn_clf->k);
}

int predict(knn_classifier_t *knn_clf, const point_t *point) {
  int ans = 0, mx = -1;

  int *result = (int *) malloc(sizeof(int) * knn_clf->k);
  int *count = (int *) calloc(1, sizeof(int) * 10);

  int found = search(knn_clf->balltree, point, result);

  // Preenche vetor de frequencias com as classes do vetor de resultados
  for (int i = 0; i < found; ++i)
    count[result[i]]++; 

  // Encontra classe com maior frequencia que sera a resposta da classificacao
  for (int i = 0; i < 10; ++i) {
    if (count[i] > mx) {
      mx = count[i];
      ans = i;
    }
  }

  free(result);
  free(count);

  return ans; 
}
