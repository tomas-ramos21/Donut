#ifndef NODE_H_
#define NODE_H_

#include "core/object.h"

/**
 * @file dataframe.h
 *
 * Contains all functionalities related with the donut's dataframe object.
 */

struct node {
        struct object;  /**< Base Object */
        char ip[40];    /**< Dataframe's files extension */
        char alias[12]; /**< Name alias for the node */
};


#endif // NODE_H_
