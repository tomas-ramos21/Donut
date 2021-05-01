#include "cmd/cmd.h"
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
 * TODO: Move Donut's folder constants to a header
 */
int
donut_init(const int argc, const struct parsed_args* args)
{
        DIR* d_ptr;
        struct dirent* dir;
        const char* donut = ".donut";
        int donut_exists = 0;
        char* path = (argc) ? args->args : ".";

        if (!(d_ptr = opendir(path))) {
                 printf("Failed to open directory.\n");
                 return 1;
        }

        while ((dir = readdir(d_ptr))) {
                if (strncmp(donut, dir->d_name, strlen(donut)) == 0) {
                        donut_exists = 1;
                        break;
                }
        }
        printf("Exists: %d\n", donut_exists);

        return 0;
}
