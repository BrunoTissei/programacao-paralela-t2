#include "core/dataset.h"

double *pack_set(set_t *set, int begin, int end) {
  int size = set->data[0]->size;
  double *packed = (double *) calloc(1, (end - begin) * ((sizeof(double) * (size)) + 
        (3 * sizeof(double))) + sizeof(double));

  int k = 0;
  packed[k++] = (double) (end - begin);
  for (int i = begin; i < end; ++i) {
    if (set->data[i] == NULL) {
      packed[k++] = 0;
      continue;
    }

    packed[k++] = (double) set->data[i]->size;
    packed[k++] = (double) set->data[i]->mclass;
    packed[k++] = (double) set->data[i]->id;

    for (int j = 0; j < set->data[i]->size; ++j) {
      packed[k++] = set->data[i]->value[j];
    }
  }

  return packed;
}

set_t *unpack_set(double *packed) {
  int k = 0;
  set_t *set = create_set((int) packed[k++]);
  
  for (int i = 0; i < set->size; ++i) {
    int size = (int) packed[k++];
    if (!size)
      continue;

    int mclass = (int) packed[k++];
    int id = (int) packed[k++];

    set->data[i] = create_point(size, id);
    set->data[i]->mclass = mclass;

    for (int j = 0; j < size; ++j) {
      set->data[i]->value[j] = packed[k++];
    }
  }

  return set;
}

point_t *create_point(uint size, int id) {
  point_t *p = (point_t *) calloc(1, sizeof(point_t));

  p->id = id;
  p->size = size;

  // Aloca memoria para valores de cada dimensao do ponto
  p->value = (double *) calloc(1, size * sizeof(double));

  return p;
}

set_t *create_set(uint size) {
  set_t *s = (set_t *) calloc(1, sizeof(set_t));

  s->size = size;

  // Aloca memoria para os pontos que pertencem ao set
  s->data = (point_t **) calloc(1, size * sizeof(point_t *));

  return s;
}

inline double distance(const point_t *a, const point_t *b) {
  int i = 0; 
  int n = a->size - (a->size % 4);

  double dist = 0.0;

  // Calcula distancia usando soma dos quadrados das diferencas
  __m256d ma, mb, msub, mans;

  for (i = 0; i < n; i += 4) {
    ma = _mm256_loadu_pd(&(a->value[i]));
    mb = _mm256_loadu_pd(&(b->value[i]));

    msub = _mm256_sub_pd(ma, mb);
    msub = _mm256_mul_pd(msub, msub);

    mans = _mm256_hadd_pd(msub, msub);
    dist += ((double *) &mans)[0] + ((double *) &mans)[2];
  }

  for ( ; i < a->size; ++i)
    dist += (a->value[i] - b->value[i]) * (a->value[i] - b->value[i]);

  return dist;
}
