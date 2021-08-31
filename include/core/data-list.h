#ifndef DATA_LIST_H_
#define DATA_LIST_H_

#include "inttypes.h"
#include "mem/slab.h"

/**
 * @file data-list.h
 *
 * Functions used to operate on the data_list structure.
 */

/**
 * Initialize a data_list structure.
 *
 * Allocates the memory required for the structure, initializes the values to 0
 * and assigns it a slab allocator.
 *
 * @param slabs Pointer to the allocator mean to bed use for memory allocations.
 * @returns Pointer to the initialized data_list structure.
 */
struct data_list* init_data_list(struct slabs* slabs);

/**
 * Add a file name to the data_list structure.
 *
 * @param data data_list object to be updated with the file name.
 * @param f_name Name of the file to be added.
 */
void add_file_to_list(struct data_list* restrict data, char* f_name);

/**
 * Determines if a name is present in the data_list.
 *
 * @param list data_list object to be checked.
 * @param str Name to be checked.
 * @returns Returns 1 if the name is found and 0 otherwise.
 */
int is_in_data_list(struct data_list* list, char* str);

/**
 * Populates the given list with all the files in the repository.
 *
 * @param list data_list object to be populated.
 * @param path String containing the path to the repository's data directory.
 */
void get_repo_data_list(struct data_list* list, char* path);

/* Unit Tests */
int test_data_list_init(void);
int test_get_data_list_index(void);
int test_add_file_to_data_list(void);

#endif // HASH_LIST_H_
