#ifndef FILE_H_
#define FILE_H_

#include "core/object.h"

/**
 * @file file.h
 *
 * Contains all functionalities related with the donut's file object.
 */

struct data_file {
        struct object; /**< Base Object */
        mode_t mode;   /**< File's mode flags */
        uint64_t size; /**< File's Size */
};

#endif // FILE_H_
