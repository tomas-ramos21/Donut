#ifndef __DECORATIONS_H_
#define __DECORATIONS_H_

#include "colour.h"

/**
 * @file decorations.h
 *
 * Contains all string decorations and messages to be used by Donut.
 */

/**
 * @def DONUT
 * The standard donut banner.
 */
#define DONUT "[Donut] "

/**
 * @def DONUT_SUCCESS
 * The donut banner be used in case of successful events.
 */
#define DONUT_SUCCESS GREEN "[Donut] " RESET

/**
 * @def DONUT_ERROR
 * The donut banner be used in case of unsuccessful events.
 */
#define DONUT_ERROR RED "[Donut] " RESET

/**
 * @def HELP_CMD
 * The string to be printed by the "help" command.
 */
#define HELP_CMD "\nThese are common Donut commands used in various situations:\n \
\n \
Starting a working area: \n \
\t - init \t Create an empty Donut container \n \
\t - chkin \t Commit data to an existing Donut container \n \
\n \
Check if your Donut isn't spoiled: \n \
\t - doctor \t Run all the unit tests to check for issues \n \
\t - conf \t Show Donut's current hardware and software configuration \n \
"

#endif // __DECORATIONS_H_
