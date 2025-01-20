#ifndef NETBUF_H_
#define NETBUF_H_

#ifdef __cplusplus
extern "C" {
#endif

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
    uint32_t id;
    union {
        struct
        {
            enum {
                CAN_FRAME_REMOTE = 0,
                CAN_FRAME_DATA = 1,
            } frame_type;
        } can_data;
    } if_data;

    size_t user_data_length;
    uint8_t user_data[];
} net_buffer_t;

/* forward decl. */
struct simple_stack;
struct circular_buffer;

typedef struct net_buffer_cb {
    size_t num_buffers;
    size_t buffer_capacity;
    struct {
        uint8_t high_water;
    } stats;
    struct simple_stack* free_list;
    struct circular_buffer* used_list;
    struct netbuffer* buffers;
} net_buffer_cb_t;

int NetBufferInit(net_buffer_cb_t* cb, size_t nElems, size_t bufSize);
int NetBufferDeinit(net_buffer_cb_t* cb);

net_buffer_t* NetBufferRequest(net_buffer_cb_t* cb);
net_buffer_t* NetBufferRequestUnchecked(net_buffer_cb_t* cb);
int NetBufferRelease(net_buffer_cb_t* cb, net_buffer_t* buffer);

/* Write `len` bytes of `data` to the buffer and set the `user_data_length` field
 * Performs validation over `len`, but does not check if `cb` or `buffer` are valid */
int NetBufferWriteChecked(net_buffer_cb_t* cb, net_buffer_t* buffer, const void* data, size_t len);

int NetBufferGetUsedCount(net_buffer_cb_t* self);

net_buffer_t* NetBufferGetLRU(net_buffer_cb_t* self);

int NetBufferUpdateCounters(net_buffer_cb_t* self);

#ifdef __cplusplus
}
#endif

#endif /* NETBUF_H_ */
