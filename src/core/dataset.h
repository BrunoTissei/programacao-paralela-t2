#ifndef _DATA_SET_H
#define _DATA_SET_H

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <immintrin.h>

typedef struct point_t {
  int size;
  int mclass, id;
  double *value;
} point_t;

typedef struct set_t {
  int size;
  point_t **data;
} set_t;


double *pack_set(set_t *set, int begin, int end);

set_t *unpack_set(double *packed);

// Retorna um ponteiro para um ponto alocado na memoria
point_t *create_point(uint size, int id);

// Retorna um ponteiro para um conjunto alocado na memoria
set_t *create_set(uint size);

// Calcula a distancia entre o ponto A e o ponto B
double distance(const point_t *a, const point_t *b);

#endif
