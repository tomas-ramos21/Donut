#include "cmd/cmd.h"
#include "unistd.h"
#include "stdio.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "sys/stat.h"

#define DIR_CTOR_MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

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
