#include "RBTree.h"
#include "gtest.h"
#include <random>
#include <numeric>
using namespace std;
static mt19937 gen(random_device{}());

TEST(RBTree, can_create_empty_rb_tree) {
	ASSERT_NO_THROW(RBTree<int> rb_tree);
}

TEST(RBTree, range_constuctor_is_correct) {
	size_t size = 100;
	vector<int> vec(size);

	for (size_t round = 0; round < 10; ++round) {
		iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

		ASSERT_NO_THROW(
			RBTree<int> rb_tree(vec.begin(), vec.end());
			sort(vec.begin(), vec.end());
			{
				auto vec_it = vec.begin();
				auto tree_it = rb_tree.begin();
				for (size_t i = 0; i < size; ++i, ++vec_it, ++tree_it)
					EXPECT_EQ(*vec_it, *tree_it);
			}
		);
	}
}