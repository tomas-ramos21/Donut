#ifndef __CMD_H_
#define __CMD_H_

#include "donut.h"

void conf(const int argc, const struct parsed_args* args);
int donut_init(const int argc, const struct parsed_args* args);
int doctor(const int argc, const struct parsed_args* args);

#endif // __CMD_H_
