#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "netbuf.h"
#include <sys/types.h>

struct circular_buffer {
    ssize_t head; /* points at the least recently inserted item */
    ssize_t tail; /* points one item after the last item inserted */
    size_t capacity; /* the size of buffer */
    size_t count; /* number of items */
    void* entry[]; /* the buffer */
};

/* allocates storage for and initializes the data structure */
struct circular_buffer* cbuf_alloc(size_t nElems);

/* deallocates storage for this data structure */
void cbuf_free(struct circular_buffer* self);

/* inserts an item at the end, moving the tail one item forward */
void cbuf_push_back(struct circular_buffer* self, void* item);

/* removes an item from the end, moving the head one item backwards */
void* cbuf_pop_back(struct circular_buffer* self);

/* inserts an item at the start, moving the head one item backward */
void cbuf_push_front(struct circular_buffer* self, void* item);

/* removes an item from the start, moving the head one item forward */
void* cbuf_pop_front(struct circular_buffer* self);

/* number of items in the buffer */
int cbuf_count(const struct circular_buffer* self);

/* please don't use this. its cursed and O(n) for lookup and then requires
 * moving all the memory that comes before it. */
int cbuf_remove(struct circular_buffer* self, void* item);

/* check if item is in the buffer */
int cbuf_contains(const struct circular_buffer * self, const void* item);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CIRCULAR_BUFFER_H_ */
