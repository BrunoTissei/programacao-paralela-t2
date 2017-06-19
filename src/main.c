#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

#include "core/dataset.h"
#include "core/knn.h"
#include "util/io.h"
#include "structure/priority_queue.h"

#define STD_TAG 0

int main(int argc, char **argv) {
  int rank, P;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &P);

  if (argc < 4) {
    if (!rank)
      print_usage();
    return 1;
  }

  int correct = 0, iteration = 0;
  int k = atoi(argv[3]);

  set_t *training_set, *testing_set_input, *testing_set;

  double start_time;
  double start, accuracy;
  double reading_time, building_time, searching_time;

  int size = 0;
  int packsize = 0;

  // ##########################################################################
  // # LEITURA DA ENTRADA
  // ##########################################################################

  if (!rank) {
    start_time = timestamp();

    printf("Reading input...\n");
    start = timestamp();

    if ((training_set = read_input_data(argv[1])) == NULL) {
      size = -1;
      printf("ERRO: arquivo %s nao encontrado.\n", argv[1]);
    }

    if ((testing_set_input = read_input_data(argv[2])) == NULL) {
      size = -1;
      printf("ERRO: arquivo %s nao encontrado.\n", argv[2]);
    } 
    
    if (!size)
      packsize = testing_set_input->data[0]->size;

    if (!size)
      size = testing_set_input->size / P;
  }

  MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&packsize, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (size == -1) {
    MPI_Finalize();
    return 0;
  }

  if (!rank) {
    reading_time = timestamp() - start;
    printf("Done!\n\n");
  }

  if (!rank) {
    int ssize = size + (testing_set_input->size % P);
    double *pack = pack_set(testing_set_input, 0, ssize);
    testing_set = unpack_set(pack);
    free(pack);

    for (int i = 1; i < P; ++i) {
      double *pack = pack_set(testing_set_input, ssize, ssize + size);
      MPI_Send(pack, size * (packsize + 3) + 1, MPI_DOUBLE, i, STD_TAG, MPI_COMM_WORLD);

      ssize += size;
      free(pack);
    }
  } else {
    double *pack = (double *) malloc(sizeof(double) * (size * (packsize + 3) + 1));

    MPI_Recv(pack, size * (packsize + 3) + 1, MPI_DOUBLE, 0, STD_TAG, MPI_COMM_WORLD, 
        MPI_STATUS_IGNORE);

    testing_set = unpack_set(pack);
    free(pack);
  }

  // ##########################################################################
  // # TREINAMENTO
  // ##########################################################################

  int sizes[4];
  int *packed_nodes;
  double *packed_radi;
  double *packed_dataset, *packed_centers;

  knn_classifier_t *knn = create_classifier(k);

  if (!rank) {
    printf("Building tree...\n");
    start = timestamp();

    // Constroi balltree com os dados da base de treinamento
    fit(knn, training_set);

    building_time = timestamp() - start;
    printf("Done!\n\n");

    // Transforma dados para enviar para os outros processos
    packed_dataset = pack_set(knn->balltree->dataset, 0, knn->balltree->dataset->size);
    packed_centers = pack_set(knn->balltree->centers, 0, knn->balltree->centers->size);
    packed_radi = knn->balltree->radi;
    packed_nodes = knn->balltree->nodes_info;

    sizes[0] = (int) packed_dataset[0];
    sizes[1] = (int) packed_centers[0];
    sizes[2] = knn->balltree->radi_size; 
    sizes[3] = knn->balltree->nodes_size; 
  }
  
  // Envia dados para os outros processos
  MPI_Bcast(sizes, 4, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank) {
    packed_dataset = (double *) malloc((sizes[0] * (packsize + 3) + 1) * sizeof(double));
    packed_centers = (double *) malloc((sizes[1] * (packsize + 3) + 1) * sizeof(double));
    packed_radi    = (double *) malloc(sizes[2] * sizeof(double));
    packed_nodes   = (int *)    malloc(sizes[3] * sizeof(int));
  }

  MPI_Bcast(packed_dataset, sizes[0] * (packsize + 3) + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(packed_centers, sizes[1] * (packsize + 3) + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(packed_radi   , sizes[2], MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(packed_nodes  , sizes[3], MPI_INT   , 0, MPI_COMM_WORLD);

  if (rank) {
    set_t *dataset = unpack_set(packed_dataset);
    set_t *centers = unpack_set(packed_centers);

    knn->balltree = get_balltree(dataset, centers, packed_radi, packed_nodes, 1);
  }

  // ##########################################################################
  // # CLASSIFICACAO
  // ##########################################################################

  if (!rank) {
    printf("Searching tree...\n");
    start = timestamp();
  }

  // Classifica todos os pontos da base de validacao
  for (int i = 0; i < testing_set->size; ++i) {
    int pred = predict(knn, testing_set->data[i]);
    if (pred == testing_set->data[i]->mclass)
      correct++;
    iteration++;
  }

  if (!rank) {
    searching_time = timestamp() - start;
    printf("Done!\n\n");
  }

  int sum_correct = 0, sum_size = 0;
  MPI_Reduce(&correct, &sum_correct, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce(&iteration, &sum_size, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  // ##########################################################################

  delete_classifier(knn);

  if (!rank) {
    accuracy = ((double) sum_correct) / ((double) sum_size);

    printf("Reading time: %.0lf ms\n", reading_time);
    printf("Building time: %.0lf ms\n", building_time);
    printf("Searching time: %.0lf ms\n\n", searching_time);

    printf("Total time: %0.lf ms\n", timestamp() - start_time);
    printf("Accuracy: %lf\n\n", accuracy);
  }

  MPI_Finalize();

  return 0;
}
