#include "cmd/doctor.h"
#include "mem/alloc.h"
#include "stdlib.h"
#include "string.h"
#include "misc/colour.h"
#include "inttypes.h"

#define NUM_ALLOC_TESTS 10

/*
 * Test the memory allocation function modules.
 */
static void
test_mem_alloc(void)
{
        void* buff;
        int i, sum = 0;

        /*  Malloc Test */
        printf("[Allocation Module]\n");
        unsigned char res_malloc[NUM_ALLOC_TESTS];
        for(i = 0; i < NUM_ALLOC_TESTS; i++){
                buff = xmalloc(4096);
                if(!buff && ((uintptr_t) buff % ALIGN) == 0)
                        res_malloc[i] = 1;
                free(buff);
        }

        for(i = 0; i < NUM_ALLOC_TESTS; i++)
                sum += res_malloc[i];

        if(sum == NUM_ALLOC_TESTS)
                printf("    " GREEN "- xmalloc: passed" RESET "\n");
        else
                printf("    " RED "- xmalloc: failed" RESET "\n");

        /* Calloc Test */
        sum = 0;
        unsigned char res_calloc[NUM_ALLOC_TESTS];
        for(int i = 0; i < NUM_ALLOC_TESTS; i++){
                buff = xcalloc(1, 4096);
                if(!buff && ((uintptr_t) buff % ALIGN) == 0)
                        res_calloc[i] = 1;
                free(buff);
        }

        for(i = 0; i < NUM_ALLOC_TESTS; i++)
                sum += res_calloc[i];

        if(sum == NUM_ALLOC_TESTS)
                printf("    " GREEN "- xcalloc: passed" RESET "\n");
        else
                printf("    " RED "- xcalloc: failed" RESET "\n");

        /* Realloc Test */
        char* dt = (char*) malloc(10);
        memset(dt, 0x1, 10);

        unsigned char res_realloc[NUM_ALLOC_TESTS];
        for(int i = 0; i < NUM_ALLOC_TESTS; i++){
                sum = 0;
                buff = xrealloc(dt, 11);

                char* buff_cp = buff;
                while (*buff_cp) {
                        sum += *buff_cp;
                        buff_cp++;
                }

                if(!buff && ((uintptr_t) buff % ALIGN) == 0 && sum == 10)
                        res_realloc[i] = 1;
                free(buff);
        }

        sum = 0;
        for(i = 0; i < NUM_ALLOC_TESTS; i++)
                sum += res_calloc[i];

        if(sum == NUM_ALLOC_TESTS)
                printf("    " GREEN "- xrealloc: passed" RESET "\n");
        else
                printf("    " RED "- xrealloc: failed" RESET "\n");
}

int
doctor(int argc, const char** argv, const unsigned int opts)
{

        test_mem_alloc();
        return 0;
}
