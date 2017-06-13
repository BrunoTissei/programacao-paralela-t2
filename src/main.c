#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "core/dataset.h"
#include "core/knn.h"
#include "util/io.h"
#include "structure/priority_queue.h"

int main(int argc, char **argv) {
  if (argc < 4) {
    print_usage();
    return 1;
  }

  int correct = 0, iteration = 0;
  int k = atoi(argv[3]);

  set_t *training_set, *testing_set;

  double start, reading_time, building_time, searching_time, accuracy;


  // ##########################################################################
  // # LEITURA DA ENTRADA
  // ##########################################################################

  printf("Reading input...\n");
  start = timestamp();

  if ((training_set = read_input_data(argv[1])) == NULL) {
    printf("ERRO: arquivo %s nao encontrado.\n", argv[1]);
    return 1;
  }

  if ((testing_set = read_input_data(argv[2])) == NULL) {
    printf("ERRO: arquivo %s nao encontrado.\n", argv[2]);
    return 1;
  }

  reading_time = timestamp() - start;
  printf("Done!\n\n");

  // ##########################################################################
  // # TREINAMENTO
  // ##########################################################################

  knn_classifier_t *knn = create_classifier(k);

  printf("Building tree...\n");
	start = timestamp();

  // Constroi balltree com os dados da base de treinamento
  fit(knn, training_set);

  building_time = timestamp() - start;
  printf("Done!\n\n");

  // ##########################################################################
  // # CLASSIFICACAO
  // ##########################################################################

  printf("Searching tree...\n");
  start = timestamp();

  // Classifica todos os pontos da base de validacao
  #pragma omp parallel for reduction (+:correct) shared(knn, testing_set)
  for (int i = 0; i < testing_set->size; ++i) {
    int pred = predict(knn, testing_set->data[i]);

    printf("%d/%d\r", iteration, testing_set->size);
    if (pred == testing_set->data[i]->mclass)
      correct++;

    #pragma omp atomic
    iteration++;
  }

  printf("%d/%d\n", testing_set->size, testing_set->size);
  
  searching_time = timestamp() - start;
  printf("Done!\n\n");

  // ##########################################################################

  delete_classifier(knn);

  accuracy = ((double) correct) / ((double) testing_set->size);

	printf("Reading time: %.0lf ms\n", reading_time);
	printf("Building time: %.0lf ms\n", building_time);
	printf("Searching time: %.0lf ms\n\n", searching_time);

  printf("Total time: %0.lf ms\n", reading_time + building_time + searching_time);
  printf("Accuracy: %lf\n\n", accuracy);

  return 0;
}
