#include "cmd/cmd.h"
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
donut_init(const int argc, const struct parsed_args* args)
{
        register int st;
        char* path = (argc) ? args->args : ".";

        st = chdir(path);
        if (st) {
                printf(DONUT "Path provided is invalid.\n");
                return -1;
        }

        st = mkdir(DONUT_FOLDER, DIR_CTOR_MODE);
        st = st | mkdir(DATA_FOLDER, DIR_CTOR_MODE);
        if (st) {
                printf(DONUT "Failed initialization.\n");
                rmdir(DONUT_FOLDER);
                rmdir(DATA_FOLDER);
                return -1;
        }

        return 0;
}
