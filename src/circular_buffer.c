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
        self->tail -= (ssize_t)self->capacity;
    }

    self->count += 1;
}

void* cbuf_pop_back(struct circular_buffer* self)
{
    NETBUF_ASSERT(self->count);

    self->tail -= 1;
    if (self->tail < 0) {
        self->tail += (ssize_t)self->capacity;
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
        self->head += (ssize_t)self->capacity;
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
        self->head -= (ssize_t)self->capacity;
    }

    self->count -= 1;
    return item;
}

/* number of items in the buffer */
int cbuf_count(const struct circular_buffer* self)
{
    return (int)self->count;
}

int cbuf_contains(const struct circular_buffer* self, const void* item)
{
    size_t found = 0;
    ssize_t idx = self->head;
    while (idx != self->head - 1) {

        /* check for a match */
        if (self->entry[idx] == item) {
            found = 1;
            break;
        }

        /* advance the iterator */
        idx = idx + 1;
        if ((size_t)idx >= self->capacity) {
            idx -= (ssize_t)self->capacity;
        }
    }

    if (!found) {
        return -1;
    }
    return (int)idx;
}

int cbuf_remove(struct circular_buffer* self, void* item)
{
    int idx = cbuf_contains(self, item);
    if (idx < 0) {
        return -1;
    }

    /* we can fix the ordering by moving [self->head, it] forwards or by moving
     * [it, self->tail] backwards. we calculate for the two possibilities and
     * pick the one which requires less time */

    ssize_t delta_head = idx - self->head;
    if (delta_head < 0) {
        delta_head += (ssize_t)self->capacity;
    }

    ssize_t delta_tail = self->tail - idx - 1;
    if (delta_tail < 0) {
        delta_tail += (ssize_t)self->capacity;
    }

    if (delta_head < delta_tail) {
        void* src = &self->entry[self->head];
        void* dst = &self->entry[self->head + 1];
        memmove(dst, src, sizeof(void*) * (size_t)delta_head);

        self->head += 1;
        if ((size_t)self->head > self->capacity) {
            self->head -= (ssize_t)self->capacity;
        }
    } else {
        void* src = &self->entry[idx + 1];
        void* dst = &self->entry[idx];
        memmove(dst, src, sizeof(void*) * (size_t)delta_tail);

        self->tail -= 1;
        if (self->tail < 0) {
            self->tail += (ssize_t)self->capacity;
        }
    }

    self->count--;
    return 0;
}

void* cbuf_peek_front(const struct circular_buffer* self)
{
    if (!cbuf_count(self)) {
        return NULL;
    }
    return self->entry[self->head];
}

/* returns the item at the back of the buffer, but does not remove it */
void* cbuf_peek_back(const struct circular_buffer* self)
{
    if (!cbuf_count(self)) {
        return NULL;
    }
    return self->entry[self->tail - 1];
}
