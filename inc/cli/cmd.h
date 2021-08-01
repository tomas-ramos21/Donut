#ifndef __CMD_H_
#define __CMD_H_

#include "donut.h"
#include "inttypes.h"

/**
 * @file cmd.h
 *
 * Header containing the declaration of all commands available in donut.
 */

/**
 * Prints the current configuration detected and being used by donut.
 *
 * @param argc Number of arguments passed
 * @param args Array of arguments
 */
void conf(const int argc, char** argv, int arg_idx, char* opts, uint64_t oflags);

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
int donut_init(const int argc, char** argv, int arg_idx, char* opts, uint64_t oflags);

/**
 * Display all unit tests results to the user.
 *
 * @param argc Number of arguments passed
 * @param args Array of arguments
 */
int doctor(const int argc, char** argv, int arg_idx, char* opts, uint64_t oflags);

int chkin(const int argc, char** argv, int arg_idx, char* opts, uint64_t oflags);

int ls_data(const int argc, char** argv, int arg_idx, char* opts, uint64_t oflags);

#endif // __CMD_H_
