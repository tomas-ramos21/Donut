#include "stdio.h"
#include "stdlib.h"
#include "donut.h"
#include "string.h"
#include "cmd/cmd.h"

#define CMD_CNT 3
#define RECURSIVE_OPT 0x1

struct parsed_args {
        const char** args;
        const unsigned int opts;
};

static int
cmd_max_args(const char* cmd, const int len)
{
        const int arg_sz[] = {1, 0, 0};
        const char* cmds[] = {"init", "doctor", "conf"};

        for (int i = 0; i < CMD_CNT; i++)
                if(strncmp(cmd, cmds[i], len) == 0)
                        return arg_sz[i];

        printf("Invalid command: %s\n", cmd);
        exit(0);
}

static struct parsed_args
parse_args(char** argv, const int max_args)
{
        char** all_args = &argv[2];
        const char* args[max_args];
        int total_args = 0;
        unsigned int opts = 0x0;

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

        struct parsed_args ret = { args, opts };
        return ret;
}

/* TODO: Replace printf with actual commands
 * TODO: Replace invalid/no command message with message displaying available commands
 */
int
donut_main(int argc, char** argv)
{
        const char* cmd;
        int len, max_args;

        if (!argv[1]) {
                printf("No command was given.\n");
                exit(0);
        }

        cmd = argv[1];
        len = strlen(cmd);
        max_args = cmd_max_args(cmd, len);
        const struct parsed_args args = parse_args(argv, max_args);


        if (strncmp("init", cmd, len) == 0)
                init(max_args, args.args, args.opts);
        else if (strncmp("doctor", cmd, len) == 0)
                doctor(max_args, args.args, args.opts);
        else if (strncmp("conf", cmd, len) == 0)
                conf(max_args, args.args, args.opts);
        else
                printf("Unrecognized command");


        return 0;
}

