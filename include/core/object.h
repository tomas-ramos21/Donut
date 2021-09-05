#ifndef OBJ_H_
#define OBJ_H_

#include "inttypes.h"

/**
 * @file object.h
 *
 * Contains all functionalities related with the base object structure.
 */

/**
 * @def OID_LENGTH
 * Length of the Object's ID.
 */
#define OID_LENGTH 65

/**
 * @def DATAFILE_OBJ
 * Bitmask which identifies a file object.
 */
#define DATAFILE_OBJ 0x1

/**
 * @def DATAFRAME_OBJ
 * Bitmask which identifies a dataframe object.
 */
#define DATAFRAME_OBJ 0x2

/**
 * @def NODE_OBJ
 * Bitmask which identifies a node object.
 */
#define NODE_OBJ 0x4

struct object {
        uint16_t oflags;     /**< Object's bit flags */
        char id[OID_LENGTH]; /**< Object's ID */
};

#endif // OBJ_H_
