#ifndef FIO_UTILS_H_
#define FIO_UTILS_H_

#include "core/data-list.h"

void get_repo_data_list(struct data_list* list, char* path);

char* xgetcwd(char* buf, size_t size);

int test_xgetcwd(void);

#endif // FIO-UTILS_H_
