#include "stdio.h"
#include "stdlib.h"
#include "donut.h"
#include "inttypes.h"
#include "string.h"

#define CMD_CNT 3

struct parsed_args {
        const char* cmd;
        const char** args;
        unsigned int opts;
};

/*
 * TODO: Replace invalid command message with message displaying available commands
 */
static int
cmd_max_args(const char* cmd)
{
        printf("Here2\n");
        if(!cmd){
                printf("No command was given.\n");
                exit(0);
        }

        const int len = strlen(cmd);
        const int arg_sz[] = {1, 1, 2};
        const char* cmds[] = {"init", "ls", "cp"};

        printf("Here3\n");
        for(int i = 0; i < CMD_CNT; i++)
                if(strncmp(cmd, cmds[i], len) == 0)
                        return arg_sz[i];

        printf("Here4\n");
        printf("Invalid command: %s\n", cmd);
        exit(0);
}

/*
 * TODO: Change for safe versions of malloc and other builting functions.
 * Also use the future SLAB allocator instead of the standard memory allocations
 */
static struct parsed_args
parse_args(char** argv)
{
        int max_args = cmd_max_args(argv[1]);
        printf("Here\n");
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

        struct parsed_args ret = {.cmd = argv[1], .args = args, .opts = opts};
        return ret;
}

int
donut_main(int argc, char** argv)
{
        printf("The ARGC: %d\n", argc);
        for(int i = 1; i < argc; i++)
                printf("Argument %d: %s\n", i, argv[i]);

        struct parsed_args cmd_args = parse_args(argv);
        printf("Command: %s\n", cmd_args.cmd);
        printf("Options: 0x%x\n", cmd_args.opts);

        return 0;
}

