#include "stdio.h"
#include "stdlib.h"
#include "donut.h"
#include "unistd.h"
#include "string.h"
#include "cli/cmd.h"
#include "cli/arg-parse.h"
#include "misc/decorations.h"
#include "mem/slob.h"
#include "inttypes.h"

int
donut_main(int argc, char** argv)
{
        size_t len;
        const char* cmd;
        uint64_t oflags = 0;
        int args_idx, ret = 0;
        struct slobs* slobs = init_slobs();
        void* buf = alloc_slob(slobs, PAGE_SIZE);

        if (!argv[1]) {
                printf(DONUT "No command was given. Use \"help\" to learn about\
 Donut.\n");
                exit(1);
        }

        cmd = argv[1];
        len = strnlen(cmd, 15);
        args_idx = parse_opts(argc, argv, buf, &oflags);

        if (!strncmp("init", cmd, len))
                ret = donut_init(argc, argv, args_idx, buf, oflags);
        else if (!strncmp("chkin", cmd, len))
                ret = chkin(argc, argv, args_idx, buf, oflags);
        else if (!strncmp("doctor", cmd, len))
                ret = doctor(argc, argv, args_idx, buf, oflags);
        else if (!strncmp("conf", cmd, len))
                conf(argc, argv, args_idx, buf, oflags);
        else if (!strncmp("ls-data", cmd, len))
                ls_data(argc, argv, args_idx, buf, oflags);
        else if (!strncmp("help", cmd, len))
                printf(HELP_CMD);
        else
                printf(DONUT_ERROR "Unrecognized command\n" HELP_CMD);

        clear_slobs(slobs);
        return ret;
}

