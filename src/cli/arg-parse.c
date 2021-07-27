#include "cli/arg-parse.h"
#include "const/const.h"
#include "misc/decorations.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

static int
is_valid_str_arg(char* str, char arg)
{
        if (strnlen(str, (MAX_ARG_SZ + 1)) >= (MAX_ARG_SZ + 1)) {
                printf(DONUT_ERROR "The argument '%c' exceeds the maximum\
characters: %d.\n", arg, MAX_ARG_SZ);
                exit(1);
        }
        return 1;
}

int
parse_opts(int argc, char** argv, void* buf, uint64_t* opt_flags)
{
        int option;
        char* str;

        while ((option = getopt((argc - 1), &argv[1], "rn:")) != -1) {
                switch (option) {
                        case 'r':
                                *opt_flags |= RECURSIVE_OPT;
                                break;
                        case 'n':
                                *opt_flags |= NAME_OPT;
                                str = (char*)buf + (NAME_ARG_IDX * (MAX_ARG_SZ + 1));
                                if (is_valid_str_arg(optarg, 'n'))
                                        strncpy(str, optarg, MAX_ARG_SZ);
                                break;
                        default:
                                break;
                }
        }

        return optind + 1;
}

int
test_parse_opts(void)
{
        uint64_t tmp = 0, ret = 0;
        int opt_idx;
        void* buf = calloc(1, 1024);

        /* Tests */
        char* args_1[4] = {"/usr/local/bin/donut", "doctor", "-n", "name"};
        char* args_2[3] = {"/usr/local/bin/donut", "doctor", "-r"};
        char* args_3[4] = {"/usr/local/bin/donut", "chkin", "-n", "name"};
        char* args_4[5] = {"/usr/local/bin/donut", "chkin", "-n", "name",
        "~/test.txt"};
        char* args_5[6] = {"/usr/local/bin/donut", "chkin", "-n", "name", "-r",
        "~/test/txt"};

        /* First Test */
        opt_idx = parse_opts(4, args_1, buf, &tmp);
        ret |= (tmp == 2) ? 1 : 0;
        ret &= (opt_idx == 4) ? 1 : 0;
        ret &= (!strncmp(buf, "name", 4)) ? 1 : 0;

        /* Second Test */
        memset(buf, 0x0, 1024);
        optind = 1;
        tmp = 0;
        opt_idx = parse_opts(3, args_2, buf, &tmp);
        ret &= (tmp == 1) ? 1 : 0;
        ret &= (opt_idx == 3) ? 1 : 0;

        /* Third Test */
        memset(buf, 0x0, 1024);
        optind = 1;
        tmp = 0;
        opt_idx = parse_opts(4, args_3, buf, &tmp);
        ret &= (tmp == 2) ? 1 : 0;
        ret &= (opt_idx == 4) ? 1 : 0;
        ret &= (!strncmp(buf, "name", 4)) ? 1 : 0;

        /* Fourth Test */
        memset(buf, 0x0, 1024);
        optind = 1;
        tmp = 0;
        opt_idx = parse_opts(5, args_4, buf, &tmp);
        ret &= (tmp == 2) ? 1 : 0;
        ret &= (opt_idx == 4) ? 1 : 0;
        ret &= (!strncmp(buf, "name", 4)) ? 1 : 0;

        /* Fifth Test */
        memset(buf, 0x0, 1024);
        optind = 1;
        tmp = 0;
        opt_idx = parse_opts(6, args_5, buf, &tmp);
        ret &= (tmp == 3) ? 1 : 0;
        ret &= (opt_idx == 5) ? 1 : 0;
        ret &= (!strncmp(buf, "name", 4)) ? 1 : 0;

        return ret;
}

void
parse_args(int argc, char** argv, int optind, void*buf)
{

}
