#include "cli/cmd.h"
#include "mem/slab.h"
#include "errno.h"
#include "unistd.h"
#include "stdio.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "sys/stat.h"

/**
 * @file init.c
 *
 * Implements all functions, utilities used by the "init" command.
 */

/**
 * @def DIR_CTOR_MODE
 * Flags used to create Donut directories.
 */
#define DIR_CTOR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

/**
 * Initializes donut in the current or provided path.
 *
 * Create the required setup to use donut in the current path or the path
 * provided in the arguments. If the donut directory already exists it will not
 * be initialized.
 *
 * @param argc Number of arguments passed
 * @param args Array of arguments
 * @returns In case of success returns 0 otherwise -1
 */
int
donut_init(const int argc, char** argv, int arg_idx, char* opts,
           uint64_t oflags)
{
        register int st;
        char* path = argv[arg_idx];

        path = (path) ? path : ".";
        st = chdir(path);
        if (st) {
                printf(DONUT_ERROR "Path provided is invalid: %s\n", path);
                return -1;
        }

        st = mkdir(DONUT_FOLDER_RELATIVE, DIR_CTOR_MODE);
        if (st && errno == EEXIST) {
                printf(DONUT_ERROR "Donut is already initialized.\n");
                exit(0);
        }

        st |= mkdir(DATA_FOLDER_RELATIVE, DIR_CTOR_MODE);
        if (st) {
                printf(DONUT_ERROR "Failed initialization.\n");
                rmdir(DONUT_FOLDER_RELATIVE);
                rmdir(DATA_FOLDER_RELATIVE);
                return -1;
        }

        return 0;
}
