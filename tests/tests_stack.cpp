#include "gtest.h"
#include "stack.h"

TEST(Stack, can_create_stack) {
	ASSERT_NO_THROW(Stack<int> {});
	ASSERT_NO_THROW(Stack<int> {Stack<int>{}});
	ASSERT_NO_THROW(Stack<int> {std::move(Stack<int>{})});
}

TEST(Stack, can_push_element) {
    Stack<int> s;
	int val = 1;
    ASSERT_NO_THROW(s.push(val));
	ASSERT_NO_THROW(s.push({3,6}));
	ASSERT_NO_THROW(s.push(5));
    EXPECT_EQ(4, s.size());
}

TEST(Stack, can_pop_element) {
    Stack<int> s;
	int val = 1;
    s.push(val);
    s.push({3,4});
    s.push(6);
    ASSERT_NO_THROW(s.pop());
    EXPECT_EQ(3, s.size());
}

TEST(Stack, test_compare_operators) {
    Stack<int> s1, s2;
    s1.push(1);
    s1.push(3);
    s1.push(6);
    s2.push(1);
    s2.push(3);
    s2.push(6);
    EXPECT_TRUE(s1 == s2);
    EXPECT_FALSE(s1 != s2);
    s2.pop();
    EXPECT_FALSE(s1 == s2);
}

TEST(Stack, can_copy_stack) {
    Stack<int> s;
    s.push(1);
    s.push(3);
    s.push(6);
    Stack<int> cop(s);
    EXPECT_EQ(s, cop);
}

TEST(Stack, can_move_stack) {
	Stack<int> s;
	s.push({1,4,5});
	Stack<int> cop(std::move(s));
	EXPECT_NE(s, cop);
}

TEST(Stack, can_swap_stack) {
	Stack<int> s, s1, s2;
	s1.push({1,4,5});
	s = s1;
	s2.push({5,6,7,8,9});
	ASSERT_NO_THROW(swap(s1,s2));
	EXPECT_EQ(s, s2);
}

TEST(Stack, can_peek) {
    Stack<int> s;
    s.push(1);
    s.push(3);
    s.push(6);
	size_t res = s.peek();
    s.~Stack();
    EXPECT_EQ(6, res);
}
