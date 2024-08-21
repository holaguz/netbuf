#include "circular_buffer.h"
#include <string.h>

struct circular_buffer* cbuf_alloc(size_t nElems)
{
    size_t totalSize = nElems * sizeof(void*) + sizeof(struct circular_buffer);
    struct circular_buffer* cb = NETBUF_MALLOC(totalSize);

    if (!cb) {
        return NULL;
    }

    cb->count = 0;
    cb->capacity = nElems;
    cb->head = cb->tail = 0;

    return cb;
}

void cbuf_free(struct circular_buffer* self)
{
    free(self);
}

void cbuf_push_back(struct circular_buffer* self, void* item)
{
    NETBUF_ASSERT(self->count < self->capacity);

    self->entry[self->tail] = item;

    self->tail += 1;
    if ((size_t)self->tail >= self->capacity) {
        self->tail -= self->capacity;
    }

    self->count += 1;
}

void* cbuf_pop_back(struct circular_buffer* self)
{
    NETBUF_ASSERT(self->count);

    self->tail -= 1;
    if (self->tail < 0) {
        self->tail += self->capacity;
    }

    void* item = self->entry[self->tail];

    self->count -= 1;
    return item;
}

void cbuf_push_front(struct circular_buffer* self, void* item)
{
    NETBUF_ASSERT(self->count < self->capacity);

    self->head -= 1;
    if (self->head < 0) {
        self->head += self->capacity;
    }

    self->entry[self->head] = item;
    self->count += 1;
}

void* cbuf_pop_front(struct circular_buffer* self)
{
    NETBUF_ASSERT(self->count);

    void* item = self->entry[self->head];

    self->head++;
    if ((size_t)self->head >= self->capacity) {
        self->head -= self->capacity;
    }

    self->count -= 1;
    return item;
}

/* number of items in the buffer */
int cbuf_count(const struct circular_buffer* self)
{
    return self->count;
}

int cbuf_remove(struct circular_buffer* self, void* item) {
    size_t found = 0;
    ssize_t it = self->head;
    while(it != self->tail) {

        /* check for a match*/
        if(self->entry[it] == item) {
            found = 1;
            break;
        }

        /* advance the iterator */
        it = it + 1;
        if((size_t)it >= self->capacity) {
            it -= self->capacity;
        }
    }

    if(!found) {
        return -1;
    }

    /* at this point, `it` points to the element to be removed */

    /* we can fix the ordering by moving [self->head, it] forwards or by moving
     * [it, self->tail] backwards. in the next lines we check which operations
     * requires time */
    int delta_head = it - self->head;
    int delta_tail = self->tail - it - 1;
    NETBUF_ASSERT(delta_head > 0);
    NETBUF_ASSERT(delta_tail > 0);

    if (delta_head < delta_tail) {
        int delta = delta_head;

        void* src = &self->entry[self->head];
        void* dst = &self->entry[self->head + 1];
        memmove(dst, src, sizeof(void*) * delta);
        self->head += 1;

        NETBUF_ASSERT(self->head < self->capacity);
        /* if ((size_t)self->head >= self->capacity) { */
        /*     self->head -= self->capacity; */
        /* } */
    }
    else {
        int delta = delta_head;

        void* src = &self->entry[it + 1];
        void* dst = &self->entry[it];
        memmove(dst, src, sizeof(void*) * delta);
        self->tail -= 1;

        NETBUF_ASSERT(self->tail > 0);
        /* if ((size_t)self->tail < 0) { */
        /*     self->tail += self->capacity; */
        /* } */
    }

    self->count--;
    return 0;
}

