#ifndef NETBUF_STACK_H_
#define NETBUF_STACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "netbuf.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

/**
 * Simple stack implementation
 * - Blazingly fast
 * - Doesn't check for self pointer validity
 * - O(1) push/pop, O(n) remove
 */

struct simple_stack {
    uint8_t is_sorted : 1;

    // current number of elements in the stack
    size_t tail_idx;

    // the maximum number of elements that can be stored
    size_t capacity;
    void* entry[];
};

#define SIMPLE_STACK_TOTAL_SIZE(capacity) (sizeof(struct simple_stack) + (capacity * sizeof(void*)))

static inline struct simple_stack* stack_alloc(size_t capacity)
{
    // allocate the stack
    struct simple_stack* q = (struct simple_stack*)NETBUF_MALLOC(SIMPLE_STACK_TOTAL_SIZE(capacity));
    if (q == NULL) {
        return NULL;
    }

    // set the memory to 0
    memset(q, 0, SIMPLE_STACK_TOTAL_SIZE(capacity));

    // set the initial values
    q->is_sorted = 1;
    q->capacity = capacity;

    return q;
}

// initialize the members of the stack. useful if the stack is not dynamically allocated!
static inline void stack_init(struct simple_stack* self, uint32_t capacity)
{
    // set the memory to 0
    memset(self, 0, SIMPLE_STACK_TOTAL_SIZE(capacity));

    // set the initial values
    self->is_sorted = 1;
    self->capacity = capacity;
}

static inline void stack_free(struct simple_stack* self)
{
    NETBUF_ASSERT(self != NULL);
    NETBUF_FREE(self);
}

static inline void* stack_pop(struct simple_stack* self)
{
    NETBUF_ASSERT(self->tail_idx > 0);
    self->tail_idx -= 1;

    // copy the entry
    void* entry = self->entry[self->tail_idx];

    // mark entry as null
    self->entry[self->tail_idx] = NULL;

    return entry;
}

static inline void stack_push(struct simple_stack* self, void* entry)
{
    NETBUF_ASSERT(self->tail_idx < self->capacity);

    // update the entry
    self->entry[self->tail_idx++] = entry;
}

// returns 0 if item removed succesfully, -1 otherwise
// if an entry is present multiple times, only the entry that was inserted first is removed
static inline int stack_remove(struct simple_stack* self, void* entry)
{
    size_t ub = self->is_sorted ? (size_t)self->tail_idx : self->capacity;
    for (size_t i = 0; i < ub; ++i) {
        if (self->entry[i] == entry) {
            self->entry[i] = NULL;
            self->is_sorted = 0;
            return 0;
        }
    }

    return -1;
}

static inline int stack_contains(struct simple_stack* self, void* entry)
{
    size_t ub = self->is_sorted ? (size_t)self->tail_idx : self->capacity;
    for (size_t i = 0; i < ub; ++i) {
        if (self->entry[i] == entry) {
            return 1;
        }
    }

    return 0;
}

// stable sort, push nulls to the back
static inline int _cmp_higher_first(const void* lhs, const void* rhs)
{
    void* _lhs = *(void**)lhs;
    void* _rhs = *(void**)rhs;

    if(_lhs == NULL) return +1;
    if(_rhs == NULL) return -1;
    return 0;

    /* return (uint8_t*)_rhs - (uint8_t*)_lhs ; */
}

static inline void stack_sort(struct simple_stack* self)
{
    qsort(self->entry, self->capacity, sizeof(void*), _cmp_higher_first);
    for(size_t i = 0; i < self->capacity; ++i) {
        if (self->entry[i] == NULL) {
            self->tail_idx = i;
            break;
        }
    }

    self->is_sorted = 1;
}

static inline size_t stack_count(struct simple_stack * self) {
    return self->tail_idx;
}

#ifdef __cplusplus
}
#endif

#endif /* NETBUF_STACK_H_ */
