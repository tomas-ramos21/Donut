#ifndef __DONUT_H_
#define __DONUT_H_

/**
 * Structure used to keep the options and arguments passed to Donut.
 */
struct parsed_args {
        unsigned int opts; /**< Options passed by the user */
        char* args;        /**< Array of arguments passed by the user*/
};

/**
 * Donut's actual entry point.
 *
 * This is the actual Donut's entry point, which should be modified in case
 * custom behaviour is required from the program's entry point. The "actual"
 * main function should not be modified under any circumstance.
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 * @returns In case of success the return value is 0 otherwise 1
 */
int donut_main(int argc, char** argv);

#endif
