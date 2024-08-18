#include "simple_stack.h"
#include <stdio.h>

int main(void)
{
    void* arr[] = {
        (void*)NULL,
        (void*)0x1,
        (void*)NULL,
        (void*)0x4,
        (void*)NULL,
        (void*)0x3,
        (void*)NULL,
        (void*)0x2,
        (void*)0x1,
        (void*)NULL,
    };

    const size_t n =  sizeof(arr) / sizeof(arr[0]);

    struct simple_stack* q = stack_alloc(16);
    for (size_t i = 0; i < n; ++i) {
        stack_push(q, arr[i]);
    }

    /* qsort(&arr[0], n, sizeof(void*), _cmp_higher_first); */
    stack_sort(q);

    for (size_t i = 0; i < n; i++) {
        /* printf("%p\n", arr[i]); */
        printf("%p\n", q->entry[i]);
    }

    stack_free(q);
}
