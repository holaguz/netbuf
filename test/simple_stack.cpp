#include <gtest/gtest.h>

#define NETBUF_ASSERT(x) EXPECT_TRUE(x)

#include "simple_stack.h"
namespace {

TEST(SimpleStack, Leak)
{
    struct simple_stack* q = stack_alloc(16);
    stack_free(q);
}

TEST(SimpleStack, Alloc)
{
    struct simple_stack* q = stack_alloc(16);
    EXPECT_EQ(q->is_sorted, 1);
    EXPECT_EQ(q->capacity, 16);
    EXPECT_EQ(q->tail_idx, 0);
    stack_free(q);
}

TEST(SimpleStack, PushPop)
{
    struct simple_stack* q = stack_alloc(16);

    void* input[] = {
        (void*)1, (void*)2, (void*)3, (void*)4
    };
    const size_t n = sizeof(input) / sizeof(input[0]);

    for (size_t i = 0; i < n; i++) {
        stack_push(q, input[i]);
    }

    EXPECT_EQ(q->tail_idx, n);

    for (int i = n - 1; i >= 0; i--) {
        EXPECT_EQ(stack_pop(q), input[i]);
    }

    EXPECT_EQ(q->entry[0], nullptr);

    stack_free(q);
}

TEST(SimpleStack, Contains)
{
    struct simple_stack* q = stack_alloc(16);

    void* input[] = {
        (void*)1, (void*)2, (void*)3, (void*)4
    };
    const size_t n = sizeof(input) / sizeof(input[0]);
    for (size_t i = 0; i < n; i++) {
        stack_push(q, input[i]);
    }

    for (size_t i = 0; i < n; i++) {
        EXPECT_TRUE(stack_contains(q, input[i]));
    }

    stack_free(q);
}

TEST(SimpleStack, Remove)
{
    struct simple_stack* q = stack_alloc(16);

    void* input[] = {
        (void*)1, (void*)2, (void*)3, (void*)4
    };
    const size_t n = sizeof(input) / sizeof(input[0]);

    for (size_t i = 0; i < n; i++) {
        stack_push(q, input[i]);
    }

    stack_remove(q, (void*)3);
    EXPECT_FALSE(q->is_sorted);
    EXPECT_FALSE(stack_contains(q, (void*)3));

    stack_free(q);
}

TEST(SimpleStack, StableSort)
{
    struct simple_stack* q = stack_alloc(16);

    void* input[] = {
        (void*)0x1,
        (void*)0x4,
        (void*)NULL,
        (void*)0x3,
        (void*)0x2,

    };
    const size_t n = sizeof(input) / sizeof(input[0]);

    for (size_t i = 0; i < n; i++) {
        stack_push(q, input[i]);
    }

    stack_sort(q);

    EXPECT_TRUE(q->is_sorted);

    EXPECT_EQ(q->entry[0], (void*)1);
    EXPECT_EQ(q->entry[1], (void*)4);
    EXPECT_EQ(q->entry[2], (void*)3);
    EXPECT_EQ(q->entry[3], (void*)2);
    EXPECT_EQ(q->entry[4], (void*)NULL);

    EXPECT_EQ(q->tail_idx, n - 1);

    stack_free(q);
}

} // namespace
