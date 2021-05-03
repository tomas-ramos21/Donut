#include "io/fio.h"
#include "stdio.h"
#include "const/err.h"
#include "stdlib.h"
#include "misc/decorations.h"
#include "sys/errno.h"
#include "sys/stat.h"
#include "unistd.h"
#include "string.h"

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

int
test_xopen(void)
{
        int ret;
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);

        /* Should be successful - All conditions are satisfied */
        system("touch donut_test.txt");
        cwd = strncat(cwd, "/donut_test.txt", 15);
        ret = xopen(cwd, O_RDWR);
        if (ret > 0) {
                close(ret);
                ret = 1;
        } else {
                ret = 0;
        }
        system("rm donut_test.txt");

        free(cwd);
        return ret;
}

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

int
test_xclose(void)
{
        int fd;
        struct stat file = {0};
        char* cwd = malloc(PAGE_SIZE);
        cwd = getcwd(cwd, PAGE_SIZE);

        /* Create a file and open it */
        system("touch donut_test.txt");
        cwd = strncat(cwd, "/donut_test.txt", 15);
        fd = open(cwd, O_RDWR);
        if (fd < 0)
                return 0;

        /* Close file and check if the descriptor is still accessible */
        xclose(fd);
        fd = fstat(fd, &file);
        if (fd == 0)
                return 0;

        system("rm donut_test.txt");
        return 1;
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
