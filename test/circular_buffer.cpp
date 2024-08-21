#include <gmock/gmock.h>
#include <numeric>
#include <ranges>

using namespace ::testing;

#define NETBUF_ASSERT(x) EXPECT_TRUE(x)

#include "circular_buffer.h"
#include "netbuf.h"

namespace {

TEST(CircularBuffer, Alloc)
{
    struct circular_buffer* cb;
    cb = cbuf_alloc(64);

    EXPECT_NE(nullptr, cb);
    EXPECT_EQ(0, cb->head);
    EXPECT_EQ(0, cb->tail);
    EXPECT_EQ(0, cb->count);
    EXPECT_EQ(64, cb->capacity);

    cbuf_free(cb);
}

TEST(CircularBuffer, PushBack)
{
    struct circular_buffer* cb;
    cb = cbuf_alloc(64);

    auto values = std::vector<size_t>(32);
    std::iota(values.begin(), values.end(), 0x69);
    /* 0x69 0x6A 0x6B ... */

    for (auto v : values) {
        cbuf_push_back(cb, (void*)v);
    }

    EXPECT_EQ(values.size(), cb->tail);
    EXPECT_EQ(values.size(), cbuf_count(cb));

    for (size_t i = 0; i < values.size(); ++i) {
        EXPECT_EQ((void*)values[i], cb->entry[i]);
    }

    cbuf_free(cb);
}

TEST(CircularBuffer, PushFront)
{
    struct circular_buffer* cb;
    cb = cbuf_alloc(64);

    auto values = std::vector<size_t>(32);
    std::iota(values.begin(), values.end(), 0x69);
    /* 0x69 0x6A 0x6B ... */

    for (auto v : values) {
        cbuf_push_front(cb, (void*)v);
    }

    EXPECT_EQ(values.size(), cbuf_count(cb));

    for (size_t i = 0; i < values.size(); ++i) {
        auto idx = (cb->capacity - i - 1) % cb->capacity;
        EXPECT_EQ((void*)values[i], cb->entry[idx]);
    }

    cbuf_free(cb);
}

TEST(CircularBuffer, PopBack)
{
    struct circular_buffer* cb;
    cb = cbuf_alloc(64);

    auto values = std::vector<size_t>(32);
    std::iota(values.begin(), values.end(), 0x69);
    /* 0x69 0x6A 0x6B ... */

    for (auto v : values) {
        cbuf_push_back(cb, (void*)v);
    }

    for (auto v : std::ranges::reverse_view(values)) {
        EXPECT_EQ((void*)v, cbuf_pop_back(cb));
    }

    cbuf_free(cb);
}

TEST(CircularBuffer, PopFront)
{
    struct circular_buffer* cb;
    cb = cbuf_alloc(64);

    auto values = std::vector<size_t>(32);
    std::iota(values.begin(), values.end(), 0x69);
    /* 0x69 0x6A 0x6B ... */

    for (auto v : values) {
        cbuf_push_back(cb, (void*)v);
    }

    for (auto v : values) {
        EXPECT_EQ((void*)v, cbuf_pop_front(cb));
    }

    cbuf_free(cb);
}

TEST(CircularBuffer, PushBackWrapping)
{
    struct circular_buffer* cb;
    cb = cbuf_alloc(64);

    auto values = std::vector<size_t>(32);
    std::iota(values.begin(), values.end(), 0x69);
    /* 0x69 0x6A 0x6B ... */

    const auto offset = 60;
    cb->head = cb->tail = offset;

    for (auto v : values) {
        cbuf_push_back(cb, (void*)v);
    }

    for(auto v: values) {
        EXPECT_EQ((void*) v, cbuf_pop_front(cb));
    }

    cbuf_free(cb);
}

TEST(CircularBuffer, PopBackWrapping)
{
    struct circular_buffer* cb;
    cb = cbuf_alloc(64);

    auto values = std::vector<size_t>(32);
    std::iota(values.begin(), values.end(), 0);

    const auto offset = 0;
    for(size_t i = 0; i < offset; ++i) {
        cbuf_push_back(cb, nullptr);
        cbuf_pop_front(cb);
    }

    for (auto v : values) {
        cbuf_push_front(cb, (void*)v);
    }

    /* ASSERT_EQ((void*)values[0], cb->entry[cb->capacity - 1]); */

    for(auto v: values) {
        std::cout << v << std::endl;
        EXPECT_EQ((void*) v, cbuf_pop_back(cb));
    }

    cbuf_free(cb);
}

TEST(CircularBuffer, PopAssert)
{
    struct circular_buffer* cb;
    const size_t n = 16;
    cb = cbuf_alloc(n);

    EXPECT_DEATH(cbuf_pop_back(cb), "");
    EXPECT_DEATH(cbuf_pop_front(cb), "");
    cbuf_free(cb);
}

TEST(CircularBuffer, PushAssert)
{
    struct circular_buffer* cb;
    const size_t n = 16;
    cb = cbuf_alloc(n);

    for(size_t i = 0; i < n; ++i) {
        cbuf_push_back(cb, nullptr);
    }

    EXPECT_DEATH(cbuf_push_back(cb, nullptr), "");
    EXPECT_DEATH(cbuf_push_front(cb, nullptr), "");
    cbuf_free(cb);
}

} // namespace
