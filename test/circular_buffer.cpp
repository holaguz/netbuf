#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <numeric>
#include <ranges>

using namespace ::testing;

#define NETBUF_ASSERT(x) ASSERT_TRUE(x)
#include "circular_buffer.h"

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

    for (auto v : values) {
        EXPECT_EQ((void*)v, cbuf_pop_front(cb));
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
    for (size_t i = 0; i < offset; ++i) {
        cbuf_push_back(cb, nullptr);
        cbuf_pop_front(cb);
    }

    for (auto v : values) {
        cbuf_push_front(cb, (void*)v);
    }

    /* ASSERT_EQ((void*)values[0], cb->entry[cb->capacity - 1]); */

    for (auto v : values) {
        EXPECT_EQ((void*)v, cbuf_pop_back(cb));
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

    for (size_t i = 0; i < n; ++i) {
        cbuf_push_back(cb, nullptr);
    }

    EXPECT_DEATH(cbuf_push_back(cb, nullptr), "");
    EXPECT_DEATH(cbuf_push_front(cb, nullptr), "");
    cbuf_free(cb);
}

TEST(CircularBuffer, Remove)
{
    struct circular_buffer* cb;
    const size_t n = 16;
    cb = cbuf_alloc(n);

    auto values = std::vector<size_t> {
        1, 2, 3, 4
    };

    for (auto v : values) {
        cbuf_push_back(cb, (void*)v);
    }

    /* --------- TAIL REMOVAL -------- */
    {
        // before:
        // v head
        // 1  2  3  4  ?
        //             ^ tail

        // after:
        // v head
        // 1  2  4  ?
        //          ^ tail

        cbuf_remove(cb, (void*)3);
        EXPECT_EQ(values.size() - 1, cb->tail);
        EXPECT_EQ(values.size() - 1, cbuf_count(cb));
        EXPECT_EQ((void*)1, cbuf_pop_front(cb));
        EXPECT_EQ((void*)2, cbuf_pop_front(cb));
        EXPECT_EQ((void*)4, cbuf_pop_front(cb));
    }

    cbuf_free(cb);

    cb = cbuf_alloc(n);
    for (auto v : values) {
        cbuf_push_back(cb, (void*)v);
    }

    /* --------- HEAD REMOVAL -------- */
    {
        // before:
        // v head
        // 1  2  3  4  ?
        //             ^ tail

        // after:
        //    v head
        // ?  1  3  4  ?
        //             ^ tail

        cbuf_remove(cb, (void*)2);
        EXPECT_EQ(1, cb->head);
        EXPECT_EQ(values.size() - 1, cbuf_count(cb));
        EXPECT_EQ((void*)1, cbuf_pop_front(cb));
        EXPECT_EQ((void*)3, cbuf_pop_front(cb));
        EXPECT_EQ((void*)4, cbuf_pop_front(cb));
    }

    cbuf_free(cb);
}

TEST(CircularBuffer, RandomRemove)
{
    const size_t num_runs = 1024*1024;

    for (size_t i = 0; i < num_runs; ++i) {
        struct circular_buffer* cb;
        const size_t n = 8;
        cb = cbuf_alloc(n);
        EXPECT_NE(nullptr, cb);

        auto values = std::vector<size_t>(n);
        std::iota(values.begin(), values.end(), 1);
        for (size_t i = 0; i < values.size(); ++i) {
            auto v = values[i];
            cbuf_push_back(cb, (void*)v);
        }

        /* 1 ~ n */
        size_t times = 1 + std::rand() % n;
        for (size_t j = 0; j < times; ++j) {

            /* printf("Have %zu items inside\n", cb->count); */

            size_t removal_idx = std::rand() % values.size();

            /* std::cout << "Removing item " << std::hex << values[removal_idx] << std::endl;
            for (size_t i = 0; i < n; ++i) {
                printf("%02zx ", (size_t)cb->entry[i]);
            }
            std::cout << std::endl; */

            /* printf("%zd, %zu, %zu\n", cb->head, cb->tail, cb->count); */
            ASSERT_EQ(0, cbuf_remove(cb, (void*)values[removal_idx]));
            /* printf("%zd, %zu, %zu\n", cb->head, cb->tail, cb->count); */

            /* for (size_t i = 0; i < n; ++i) {
                printf("%02zx ", (size_t)cb->entry[i]);
            }
            std::cout << std::endl << std::endl; */

            const auto it = values.begin() + removal_idx;
            values.erase(it);

            /* printf("vec: ");
            for(auto v: values) { printf("%zu ", v); }
            printf("\n"); */

            for (size_t i = 0; i < values.size(); ++i) {
                auto v = values[i];
                auto cb_idx = (cb->head + i) % cb->capacity;
                ASSERT_EQ((void*)v, cb->entry[cb_idx]);
            }
        }
        cbuf_free(cb);
    }
}

TEST(CircularBuffer, PeekFront)
{
    struct circular_buffer* cb;
    const size_t n = 16;
    cb = cbuf_alloc(n);

    auto values = std::vector<size_t> {
        1, 2, 3, 4
    };

    for (auto v : values) {
        cbuf_push_back(cb, (void*)v);
    }

    EXPECT_EQ((void*)1, cbuf_peek_front(cb));
    cbuf_pop_front(cb);
    EXPECT_EQ((void*)2, cbuf_peek_front(cb));
    cbuf_pop_front(cb);
    EXPECT_EQ((void*)3, cbuf_peek_front(cb));
    cbuf_pop_front(cb);
    EXPECT_EQ((void*)4, cbuf_peek_front(cb));
    cbuf_pop_front(cb);

    cbuf_free(cb);
}

TEST(CircularBuffer, PeekBack)
{
    struct circular_buffer* cb;
    const size_t n = 16;
    cb = cbuf_alloc(n);

    auto values = std::vector<size_t> {
        1, 2, 3, 4
    };

    for (auto v : values) {
        cbuf_push_back(cb, (void*)v);
    }

    EXPECT_EQ((void*)4, cbuf_peek_back(cb));
    cbuf_pop_back(cb);
    EXPECT_EQ((void*)3, cbuf_peek_back(cb));
    cbuf_pop_back(cb);
    EXPECT_EQ((void*)2, cbuf_peek_back(cb));
    cbuf_pop_back(cb);
    EXPECT_EQ((void*)1, cbuf_peek_back(cb));
    cbuf_pop_back(cb);

    cbuf_free(cb);
}

} // namespace
