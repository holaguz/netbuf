#ifndef NETBUF_H_
#define NETBUF_H_

#include <stdint.h>

#ifndef NETBUF_ASSERT
#include <assert.h> // IWYU pragma: keep
#define NETBUF_ASSERT(x) assert(x)
#endif

#ifndef NETBUF_MALLOC
#include <stdlib.h> // IWYU pragma: keep
#define NETBUF_MALLOC(x) malloc(x)
#endif

#ifndef NETBUF_FREE
#define NETBUF_FREE(x) free(x)
#endif

typedef struct netbuffer {
    int8_t if_type;
    int8_t if_id;
    union {
        void* opaque;
    } if_data;

    size_t user_data_length;
    size_t user_data[];
} net_buffer_t;

typedef struct net_buffer_cb {
    size_t num_buffers;
    size_t buffer_capacity;
    struct simple_stack * free_list;
    struct simple_stack * used_list;
    struct netbuffer * buffers;
} net_buffer_cb_t;

int NetBufferInit(net_buffer_cb_t* cb, size_t nElems, size_t bufSize);
int NetBufferDeinit(net_buffer_cb_t* cb);

#endif /* NETBUF_H_ */
