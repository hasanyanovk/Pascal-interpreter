#include "gtest.h"
#include "list.h"

TEST(List, can_create_list) {
	ASSERT_NO_THROW(List<int>{});
	ASSERT_NO_THROW(List<int>({1, 2, 3, 5, 7}));
	ASSERT_NO_THROW(List<int>{List<int>{}});
	ASSERT_NO_THROW(List<int> {std::move(List<int> {})} );//not need
}

TEST(List, can_add_element) {
	List<int> list;
	ASSERT_NO_THROW(list.push_back({1, 2, 3}));
	ASSERT_NO_THROW(list.push_front({6, 7, 8}));
	EXPECT_EQ(6, list.size());
}

TEST(List, can_pop_element) {
	List<int> list;
	ASSERT_NO_THROW(list.push_back({1, 2, 3}));
	ASSERT_NO_THROW(list.pop_back());
	ASSERT_NO_THROW(list.pop_front());
	EXPECT_EQ(2, list.back());
	EXPECT_EQ(1, list.size());
}

TEST(List, can_compare_lists) {
	List<int> list1, list2;
	list1.push_back({1, 2, 3});
	list2.push_back({1, 2, 3});
	EXPECT_TRUE(list1 == list2);
	EXPECT_FALSE(list1 != list2);
}

TEST(List, can_clear_list) {
	List<int> list;
	list.push_back({1, 2, 3});
	ASSERT_NO_THROW((list.clear()));
	EXPECT_EQ(0, list.size());
}

TEST(List, can_swap_lists){
	List<int> list1, list2;
	list1.push_back({1,2,3});
	list2.push_back({4,5,6,7});
	ASSERT_NO_THROW(swap(list1, list2));
	EXPECT_EQ(4, list1.size());
	EXPECT_EQ(3, list2.size());
}

TEST(List, can_find_element){
	List<int> list;
	list.push_back({1, 2, 3, 4, 5, 6, 4, 3, 3});
	ASSERT_NO_THROW(list.find(5));
	EXPECT_TRUE(list.contains(2));
	EXPECT_FALSE(list.contains(0));
}

TEST(List, test_iterator_methods) {
	List<int> list;
	list.push_back({1, 2, 3, 4, 5, 6, 4, 3, 3});
	for(auto& a : list){
		a++;
	}// 2 3 4 5 6 7 5 4 4
	list.insert(list.begin(), {8, 4, 0});
	// 2 8 4 0 3 4 5 6 7 5 4 4
	list.remove(4);
	EXPECT_EQ(8, list.size());
	// 2 8 0 3 5 6 7 5
	std::cout <<  list << std::endl;
	list.erase(list.end());
	list.erase(list.begin());
	EXPECT_EQ(7, list.size());
	// 8 0 3 5 6 7 5
	std::cout <<  list << std::endl;
	list.erase(list.find(4));
	list.erase(list.find(0), list.find(0) + 3);
	EXPECT_EQ(4, list.size());
	// 8 6 7
	std::cout <<  list << std::endl;
	list.erase(list.begin(), list.end());
	list.remove(4);
	EXPECT_EQ(0, list.size());
	std::cout << list<< std::endl;
}


