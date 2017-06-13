#ifndef _IO_H
#define _IO_H

#include <stdio.h>
#include <sys/time.h>

#include "core/dataset.h"

// Imprime uso correto do programa
void print_usage(void);

// Le entrada fornecida por parametro
set_t *read_input_data(char *filename);

// Retorna tempo atual em milissegundos
double timestamp(void);

#endif
