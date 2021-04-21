#ifndef __DONUT_H_
#define __DONUT_H_

struct parsed_args {
        unsigned int opts;
        char* args;
};

int donut_main(int argc, char** argv);

#endif
