#ifndef DATAFRAME_H_
#define DATAFRAME_H_

#include "core/object.h"

/**
 * @file dataframe.h
 *
 * Contains all functionalities related with the donut's dataframe object.
 */

struct dataframe {
        struct object obj; /**< Base Object */
        char ext[61];      /**< Dataframe's files extension */
};

#endif // DATAFRAME_H_
