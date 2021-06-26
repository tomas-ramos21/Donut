#include "stdio.h"
#include "stdlib.h"
#include "donut.h"
#include "string.h"
#include "cli/cmd.h"
#include "misc/decorations.h"
#include "mem/slab.h"
#include "inttypes.h"

#define CMD_CNT 5
#define MAX_ARG_SIZE 1024
#define RECURSIVE_OPT 0x1

static int
cmd_max_args(const char* cmd, const int len)
{
        const int arg_sz[] = {1, 0, 0, 0, 1};
        const char* cmds[] = {"init", "doctor", "conf", "help", "chkin"};

        for (int i = 0; i < CMD_CNT; i++)
                if(strncmp(cmd, cmds[i], len) == 0)
                        return arg_sz[i];

        printf(DONUT_ERROR "Invalid command: %s\n", cmd);
        exit(0);
}

static struct parsed_args*
parse_args(const int arg_cp, char** argv, const int max_args, struct slabs* slabs)
{
        char** all_args = &argv[2];
        char* args[max_args];
        int total_args = 0;
        unsigned int opts = 0x0;

        uintptr_t* slab = alloc_slab(slabs, (MAX_ARG_SIZE * max_args) + sizeof(unsigned int));
        struct parsed_args* ret = (struct parsed_args*)slab;
        slab += sizeof(struct parsed_args);
        ret->args = (char*)slab;

        while (*all_args) {
                if (total_args < max_args && *(*all_args) != '-') {
                        args[total_args] = *all_args;
                        total_args++;
                } else {
                        if (!strncmp(*all_args, "-r", 2))
                                opts = opts | RECURSIVE_OPT;
                }
                all_args++;
        }

        ret->opts = opts;
        char* cp = ret->args;
        for (int i = 0; (i < max_args) && arg_cp; i++) {
                strncpy(cp, args[i], MAX_ARG_SIZE);
                cp += MAX_ARG_SIZE;
        }
        return ret;
}

int
donut_main(int argc, char** argv)
{
        int ret = 0;
        const char* cmd;
        int len, max_args, argc_cp = argc - 2;
        struct slabs* slabs = init_slabs();

        if (!argv[1]) {
                printf(DONUT "No command was given.\n");
                exit(0);
        }

        cmd = argv[1];
        len = strlen(cmd);
        max_args = cmd_max_args(cmd, len);
        const struct parsed_args* args = parse_args(argc_cp, argv, max_args, slabs);

        if (strncmp("init", cmd, len) == 0)
                ret = donut_init(argc_cp, args);
        else if (strncmp("chkin", cmd, len) == 0)
                ret = chkin(argc_cp, args);
        else if (strncmp("doctor", cmd, len) == 0)
                ret = doctor(argc_cp, args);
        else if (strncmp("conf", cmd, len) == 0)
                conf(argc_cp, args);
        else if (strncmp("help", cmd, len) == 0)
                printf(HELP_CMD);
        else
                printf(DONUT_ERROR "Unrecognized command\n" HELP_CMD);

        clear_slabs(slabs);
        return ret;
}

