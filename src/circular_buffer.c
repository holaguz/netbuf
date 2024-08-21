#include "circular_buffer.h"

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

