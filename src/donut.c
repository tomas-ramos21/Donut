#include "stdio.h"
#include "stdlib.h"
#include "donut.h"
#include "inttypes.h"
#include "string.h"

#define CMD_CNT 3

struct parsed_args {
        const char* cmd;
        const char** args;
        unsigned int argc;
        unsigned int opts;
};

/*
 * TODO: Replace invalid/no command message with message displaying available commands
 */
static int
cmd_max_args(const char* cmd)
{
        if(!cmd){
                printf("No command was given.\n");
                exit(0);
        }

        const int len = strlen(cmd);
        const int arg_sz[] = {1, 1, 2};
        const char* cmds[] = {"init", "ls", "cp"};

        for(int i = 0; i < CMD_CNT; i++)
                if(strncmp(cmd, cmds[i], len) == 0)
                        return arg_sz[i];

        printf("Invalid command: %s\n", cmd);
        exit(0);
}

static struct parsed_args
parse_args(char** argv)
{
        int max_args = cmd_max_args(argv[1]);
        char** all_args = &argv[2];
        const char* args[max_args];
        int total_args = 0;
        unsigned int opts = 0x0;

        while(*all_args){
                if(total_args < max_args && *(*all_args) != '-'){
                        args[total_args] = *all_args;
                        total_args++;
                } else {
                        if(!strncmp(*all_args, "-r", 2))
                                opts = opts | 0x1;
                }
                all_args++;
        }

        struct parsed_args ret = {.cmd = argv[1], .args = args, .argc = total_args, .opts = opts};
        return ret;
}

int
donut_main(int argc, char** argv)
{
        struct parsed_args cmd_args = parse_args(argv);
        printf("Command: %s\n", cmd_args.cmd);
        printf("Options: 0x%x\n", cmd_args.opts);
        for(unsigned int i = 0; i < cmd_args.argc; i++)
                printf("Argument %u: %s\n", i, cmd_args.args[i]);
        return 0;
}

