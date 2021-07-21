#ifndef __CONST_H_
#define __CONST_H_

/**
 * @file const.h
 *
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
#define DATA_FOLDER "/.donut/data"

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

#endif // __CONST_H_
