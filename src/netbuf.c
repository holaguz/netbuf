#include "netbuf.h"
#include "simple_stack.h"
#include <assert.h>

int NetBufferInit(net_buffer_cb_t* cb, size_t nElems, size_t bufSize)
{
    if (!cb || !nElems || !bufSize) {
        return -1;
    }

    cb->free_list = stack_alloc(nElems);
    cb->used_list = stack_alloc(nElems);
    cb->buffers = NETBUF_MALLOC(nElems * (sizeof(net_buffer_t) + bufSize));

    // clang-format off
    if (!cb->buffers)   { goto cleanup; }
    if (!cb->free_list) { goto cleanup; }
    if (!cb->used_list) { goto cleanup; }
    // clang-format on

    cb->num_buffers = nElems;
    cb->buffer_capacity = bufSize;

    for (size_t i = 0; i < nElems; ++i) {
        stack_push(cb->free_list, &cb->buffers[i]);
    }

    return 0;
cleanup:
    (void)NetBufferDeinit(cb);
    return -1;
}

int NetBufferDeinit(net_buffer_cb_t* cb)
{
    if (!cb) {
        return -1;
    }

    // clang-format off
    if (cb->buffers)   { NETBUF_FREE(cb->buffers),   cb->buffers   = 0; }
    if (cb->free_list) { NETBUF_FREE(cb->free_list), cb->free_list = 0; }
    if (cb->used_list) { NETBUF_FREE(cb->used_list), cb->used_list = 0; }
    // clang-format on
    return 0;
}
