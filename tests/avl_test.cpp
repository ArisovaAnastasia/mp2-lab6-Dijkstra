#include "AVLTree.h"
#include "gtest.h"
#include <algorithm>
#include <random>
#include <numeric>
using namespace std;
static mt19937 gen(random_device{}());

TEST(AVLTree, can_create_empty_avl_tree) {
	ASSERT_NO_THROW(AVLTree<int> tree);
}

TEST(AVLTree, range_constuctor_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);

	for (size_t round = 0; round < 10; ++round) {
		iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

		ASSERT_NO_THROW(
			AVLTree<int> tree(vec.begin(), vec.end());
			sort(vec.begin(), vec.end());
			{
				auto vec_it = vec.begin();
				auto tree_it = tree.begin();
				for (size_t i = 0; i < size; ++i, ++vec_it, ++tree_it)
					EXPECT_EQ(*vec_it, *tree_it);
			}
		);
	}
}

TEST(AVLTree, copy_constructor_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree1(vec.begin(), vec.end());
			AVLTree<int> tree2(tree1);
			{
				auto tree1_it = tree1.begin();
				auto tree2_it = tree2.begin();
				for (; tree1_it != tree1.end(); ++tree1_it, ++tree2_it)
					EXPECT_EQ(*tree1_it, *tree2_it);
			}
		);
	}
}


TEST(AVLTree, move_constructor_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree1(vec.begin(), vec.end());
			AVLTree<int> tree2(std::move(tree1));

			sort(vec.begin(), vec.end());
			{
				auto vec_it = vec.begin();
				auto tree_it = tree2.begin();
				for (size_t i = 0; i < size; ++i, ++vec_it, ++tree_it)
					EXPECT_EQ(*vec_it, *tree_it);
			}
		);
	}
}

TEST(AVLTree, copy_assignment_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree1(vec.begin(), vec.end());
			AVLTree<int> tree2;
			tree2 = tree1;
			{
				auto tree1_it = tree1.begin();
				auto tree2_it = tree2.begin();
				for (; tree1_it != tree1.end(); ++tree1_it, ++tree2_it)
					EXPECT_EQ(*tree1_it, *tree2_it);
			}
		);
	}
}

TEST(AVLTree, move_assignment_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree1(vec.begin(), vec.end());
			AVLTree<int> tree2;
			tree2 = std::move(tree1);

			sort(vec.begin(), vec.end());
			{
				auto vec_it = vec.begin();
				auto tree_it = tree2.begin();
				for (size_t i = 0; i < size; ++i, ++vec_it, ++tree_it)
					EXPECT_EQ(*vec_it, *tree_it);
			}
		);
	}
}

TEST(AVLTree, find_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree(vec.begin(), vec.end());

			shuffle(vec.begin(), vec.end(), gen);
			for (const auto &elem: vec) {
				EXPECT_NE(tree.find(elem), tree.end());
				EXPECT_EQ(*tree.find(elem), elem);
			}
		);
	}
}

TEST(AVLTree, insert_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree;
			for (const auto &elem: vec) {
				tree.insert(elem);
				EXPECT_NE(tree.find(elem), tree.end());
				EXPECT_EQ(*tree.find(elem), elem);
			}
		);
	}
}

TEST(AVLTree, erase_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree(vec.begin(), vec.end());

			shuffle(vec.begin(), vec.end(), gen);
			for (const auto &elem: vec) {
				tree.erase(elem);
				EXPECT_EQ(tree.find(elem), tree.end());
			}
		);
	}
}

TEST(AVLTree, size_is_correct) {
	size_t size = 1000;
	size_t curr_size; // текущий размер дерева
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		curr_size = 0;
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree;
			for (const auto &elem: vec) {
				tree.insert(elem);
				++curr_size;
				EXPECT_EQ(tree.size(), curr_size);
			}

			shuffle(vec.begin(), vec.end(), gen);
			for (const auto &elem: vec) {
				tree.erase(elem);
				--curr_size;
				EXPECT_EQ(tree.size(), curr_size);
			}
		);
	}
}

TEST(AVLTree, empty_is_correct) {
	ASSERT_NO_THROW(
		AVLTree<int> tree;
	EXPECT_EQ(tree.empty(), true);

	tree.insert(3);
	EXPECT_EQ(tree.empty(), false);

	tree.erase(3);
	EXPECT_EQ(tree.empty(), true);
	);
}

TEST(AVLTree, clear_is_correct) {
	size_t size = 1000;
	vector<int> vec(size);
	iota(vec.begin(), vec.end(), 1); // случайная перестановка чисел {1, 2, ... , size}

	for (size_t round = 0; round < 10; ++round) {
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			AVLTree<int> tree(vec.begin(), vec.end());
			EXPECT_NE(tree.size(), 0);
			tree.clear();
			EXPECT_EQ(tree.size(), 0);
		);
	}
}