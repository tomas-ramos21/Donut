#include "cmd/doctor.h"
#include "mem/alloc.h"
#include "stdlib.h"
#include "misc/colour.h"

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
}

int
doctor(int argc, const char** argv, const unsigned int opts)
{

        test_mem_alloc();
        return 0;
}
