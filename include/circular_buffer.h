#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include "netbuf.h"

#include <stdlib.h>
#include <string.h>

struct circular_buffer {
    ssize_t head; /* points at the least recently inserted item */
    ssize_t tail; /* points one item after the last item inserted */
    size_t capacity; /* the size of buffer */
    size_t count; /* number of items */
    void* entry[]; /* the buffer */
};

/* allocates storage for and initializes the data structure */
static inline struct circular_buffer* cbuf_alloc(size_t nElems, size_t itemSize)
{
    size_t totalSize = nElems * itemSize + sizeof(struct circular_buffer);
    struct circular_buffer* cb = NETBUF_MALLOC(totalSize);

    if (!cb) {
        return NULL;
    }

    cb->count = 0;
    cb->capacity = nElems;
    cb->head = cb->tail = 0;

    return cb;
}

/* deallocates storage for this data structure */
static inline void cbuf_free(struct circular_buffer* self)
{
    free(self);
}

/* inserts an item at the end, moving the tail one item forward */
static inline void cbuf_push_back(struct circular_buffer* self, void* item)
{
    NETBUF_ASSERT(self->count < self->capacity);

    self->entry[self->tail] = item;
    self->tail += 1;

    if ((size_t)self->tail >= self->capacity) {
        self->tail -= self->capacity;
    }

    self->count += 1;
}

/* removes an item from the end, moving the head one item backwards */
static inline void* cbuf_pop_back(struct circular_buffer* self)
{
    NETBUF_ASSERT(self->count);

    void* item = self->entry[self->tail];
    self->tail -= 1;
    if (self->tail < 0) {
        self->tail += self->capacity;
    }

    self->count -= 1;
    return item;
}

/* inserts an item at the start, moving the head one item backward */
static inline void cbuf_push_front(struct circular_buffer* self, void* item)
{
    NETBUF_ASSERT(self->count < self->capacity);

    self->head -= 1;
    if (self->head < 0) {
        self->head += self->capacity;
    }

    self->entry[self->head] = item;
    self->count += 1;
}

/* removes an item from the start, moving the head one item forward */
static inline void* cbuf_pop_front(struct circular_buffer* self)
{
    NETBUF_ASSERT(self->count);

    void* item = self->entry[self->head];
    if ((size_t)self->head >= self->capacity) {
        self->head -= self->capacity;
    }

    self->count -= 1;
    return item;
}

/* number of items in the buffer */
static inline int cbuf_count(const struct circular_buffer* self)
{
    return self->count;
}

/* please don't use this. its cursed and O(n) for lookup and then requires
 * moving all the memory that comes before it. */
/* static inline int cbuf_remove(struct circular_buffer* self, void* item); */

#endif /* CIRCULAR_BUFFER_H_ */
