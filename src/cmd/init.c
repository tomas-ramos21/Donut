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
init(int argc, const char** argv, const unsigned int opts)
{
        char* path = xmalloc(256);
        char* arg = xmalloc(256);
        strncpy(arg, *argv, 255);
        printf("Path: %s\n", arg);

        if (*argv)
                strncpy(path, arg, 255);
        else
                *path = '.';
        printf("Path: %s\n", path);

        /* DIR* d_ptr; */
        /* struct dirent* dir; */

        /* if (!(d_ptr = opendir("."))) { */
        /*         printf("Failed to open directory.\n"); */
        /*         return 1; */
        /* } */

        /* while ((dir = readdir(d_ptr))) { */
        /*         printf("File name: %s\n", dir->d_name); */
        /* } */

        return 0;
}
