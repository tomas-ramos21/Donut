#ifndef __COLOUR_H_
#define __COLOUR_H_

/**
 * @file colour.h
 *
 * Contains all strings which will change the colours used by the terminal.
 */

/**
 * @def RESET
 * String constant that reset text to the standard color.
 */
#define RESET   "\033[0m"

/**
 * @def BLACK
 * String constant that sets the text to black.
 */
#define BLACK   "\033[30m"

/**
 * @def RED
 * String constant that sets the text to red.
 */
#define RED     "\033[31m"

/**
 * @def GREEN
 * String constant that sets the text to green.
 */
#define GREEN   "\033[32m"

/**
 * @def YELLOW
 * String constant that sets the text to yellow.
 */
#define YELLOW  "\033[33m"

/**
 * @def BLUE
 * String constant that sets the text to blue.
 */
#define BLUE    "\033[34m"

/**
 * @def MAGENTA
 * String constant that sets the text to magenta.
 */
#define MAGENTA "\033[35m"

/**
 * @def CYAN
 * String constant that sets the text to cyan.
 */
#define CYAN    "\033[36m"

/**
 * @def WHITE
 * String constant that sets the text to white.
 */
#define WHITE   "\033[37m"

#endif // __COLOUR_H_
