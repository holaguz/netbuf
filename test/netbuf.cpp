#include <gmock/gmock.h>

using namespace ::testing;

#define NETBUF_ASSERT(x) EXPECT_TRUE(x)

#include "netbuf.h"
#include "simple_stack.h"

namespace {

TEST(NetBuffer, Leak)
{
    net_buffer_cb_t cb;
    NetBufferInit(&cb, 4, 16);
    NetBufferDeinit(&cb);
}

TEST(NetBuffer, Init)
{
    net_buffer_cb_t cb;
    NetBufferInit(&cb, 4, 16);

    /* number of buffers and size */
    EXPECT_EQ(cb.num_buffers, 4);
    EXPECT_EQ(cb.buffer_capacity, 16);

    /* free list should be full, used list should be empty */
    EXPECT_EQ(stack_count(cb.free_list), 4);
    EXPECT_EQ(stack_count(cb.used_list), 0);

    NetBufferDeinit(&cb);
}

TEST(NetBuffer, Request)
{
    net_buffer_cb_t cb[1];
    NetBufferInit(cb, 1, 16);

    /* error on invalid cb */
    EXPECT_EQ(NULL, NetBufferRequest(NULL));
    (void)NetBufferRequest(cb);

    /* error on no buffers */
    EXPECT_EQ(NULL, NetBufferRequest(cb));

    NetBufferDeinit(cb);
}

TEST(NetBuffer, Release)
{
    net_buffer_cb_t cb[1];
    NetBufferInit(cb, 1, 16);

    auto buffer = NetBufferRequest(cb);
    EXPECT_TRUE(buffer);
    EXPECT_EQ(0, NetBufferRelease(cb, buffer));

    NetBufferDeinit(cb);
}

TEST(NetBuffer, WriteChecked)
{
    net_buffer_cb_t cb[1];
    NetBufferInit(cb, 1, 16);

    const size_t data_size = 15;
    auto user_data = (uint8_t*)malloc(data_size);

    auto buffer = NetBufferRequest(cb);
    NetBufferWriteChecked(cb, buffer, user_data, data_size);

    for(size_t i = 0; i < data_size; ++i) {
        EXPECT_EQ(buffer->user_data[i], user_data[i]);
    }

    EXPECT_EQ(data_size, buffer->user_data_length);
    auto re = testing::internal::RE(".*");
    EXPECT_TRUE(re.FullMatch("sdfjksdkf", re));

    free(user_data);
    NetBufferDeinit(cb);
}
} // namespace
