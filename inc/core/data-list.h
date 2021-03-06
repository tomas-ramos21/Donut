#ifndef DATA_LIST_H_
#define DATA_LIST_H_

#include "inttypes.h"
#include "mem/slab.h"

struct data_list* init_data_list(struct slabs* slabs);
void add_file_to_list(struct data_list* restrict data, char* f_name);

/* Unit Tests */
int test_data_list_init(void);
int test_add_file_to_data_list(void);

#endif // HASH_LIST_H_
