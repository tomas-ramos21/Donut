#include "io/fio.h"
#include "stdio.h"
#include "const/err.h"
#include "stdlib.h"
#include "misc/decorations.h"
#include "sys/errno.h"
#include "unistd.h"

/*
 * TODO: Add unit tests
 */
int
xopen(const char* path, int oflag)
{
        int fd;

        if (oflag < O_RDONLY || oflag > O_RDWR) {
                printf(DONUT "Invalid flags passed for file opening.\n");
                exit(DEF_ERR);
        }

        while (1) {

                fd = open(path, oflag);
                if (fd > 0)
                        return fd;
                else if (errno == EINTR)
                        continue;

                printf(DONUT "Failed to open file: %s\n", path);
                exit(DEF_ERR);
        }
}

/*
 * TODO: Add unit tests
 */
int
xclose(int fd)
{
        while (1) {
                int ret = close(fd);

                if (EBADF == errno)
                        printf(DONUT "Tried to close invalid file descriptor.\n");
                else if (EINTR == errno)
                        continue;

                return ret;
        }
}

/*
 * TODO: Add unit tests
 * TODO: Check assembly. Might need "restrict" or re-order loop instructions
 */
size_t
xread(int fd, void* buf, size_t nbyte)
{
        size_t bytes_read;
        size_t bytes = nbyte;

        while (bytes) {
                bytes_read = read(fd, buf, bytes);

                if(errno == EINTR) {
                        continue;
                } else if (!bytes_read) {
                        return nbyte;
                } else if (bytes_read < 0) {
                        printf(DONUT "Failed reading from file with error: %lu.\n",
                               bytes_read);
                        exit(DEF_ERR);
                }

                bytes = bytes - bytes_read;
        }
        return nbyte;
}

/*
 * TODO: Add unit tests
 * TODO: Check assembly. Might need "restrict" or re-order loop instructions
 */
size_t
xwrite(int fd, void* buf, size_t nbyte)
{
        size_t written;

        while (nbyte) {
                written = write(fd, buf, nbyte);

                if(errno == EINTR) {
                        continue;
                } else if (written < 0) {
                        printf(DONUT "Failed writing to file with error: %lu.\n",
                               written);
                        exit(DEF_ERR);
                }

                nbyte = nbyte - written;

        }

        return nbyte;
}
