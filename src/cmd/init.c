#include "cmd/init.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "mem/alloc.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "dirent.h"
#include "string.h"

/*
 * TODO: Implement Donut init function
 */
int
donut_init(const int argc, const struct parsed_args* args)
{
        char* path = ".";
        if (argc)
                path = args->args;

        DIR* d_ptr;
        struct dirent* dir;

        if (!(d_ptr = opendir(path))) {
                 printf("Failed to open directory.\n");
                 return 1;
        }

         while ((dir = readdir(d_ptr))) {
                 printf("File name: %s\n", dir->d_name);
         }

        return 0;
}
