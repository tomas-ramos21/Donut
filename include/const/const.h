#ifndef __CONST_H_
#define __CONST_H_

/**
 * @file const.h
 * Contains all constants to be used in Donut and their documentation.
 */

/**
 * @def DONUT_FOLDER_RELATIVE
 * Relative path to donut's folder where all it's information is stored.
 */
#define DONUT_FOLDER_RELATIVE ".donut"

/**
 * @def DONUT_FOLDER
 * Donut's folder to be appended to path strings.
 */
#define DONUT_FOLDER "/.donut"

/**
 * @def DATA_FOLDER_RELATIVE
 * Relative path to donut's folder where all the data is stored.
 */
#define DATA_FOLDER_RELATIVE ".donut/data"

/**
 * @def DATA_FOLDER
 * Donut's data folder to be appended to path strings.
 */
#define DATA_FOLDER "/.donut/data/"

/**
 * @def CONFIG_FILE
 * File where all Donut's configuration is stored.
 */
#define CONFIG_FILE "config"

/**
 * @def DATA_FOLDER_NAME
 * The name of the folder where all the data is stored.
 */
#define DATA_FOLDER_NAME "data"

/**
 * @def TEST_FILE
 * Name of the file to be used for testing purposes.
 */
#define TEST_FILE "donut_test.txt"

/**
 * @def DATA_FILE_NAME_SIZE
 * Number of characters used in data files including the null character.
 */
#define DATA_FILE_NAME_SIZE 32

/**
 * @def MAX_ARG_SZ
 * Number of characters allowed for optional argument size.
 */
#define MAX_ARG_SZ 127

/**
 * @def NAME_ARG_IDX
 * Index of the optional argument value.
 */
#define NAME_ARG_IDX 0

/**
 * @def RECURSIVE_OPT
 * Bit that is set when the recursive option is selected.
 */
#define RECURSIVE_OPT 0x1

/**
 * @def NAME_OPT
 * Bit that is set when the name option is selected.
 */
#define NAME_OPT 0x2

/**
 * @def DEFAULT_DF
 * Name of the default dataframe.
 */
#define DEFAULT_DF "main"

#endif // __CONST_H_
