#include "gtest.h"
#include "Heap.h"
#include <iostream>
#include <random>
using namespace std;

template <typename Value>
using BHeapMin = DHeap<Value,2>;
template <typename Value>
using BHeapMax = DHeap<Value,2,greater<Value>>;

static mt19937 gen(random_device{}());

TEST(DHeap, can_create_empty_heap) {
	ASSERT_NO_THROW(BHeapMin<int> heap);
}

TEST(DHeap, can_create_empty_heap_with_non_default_comparator) {
	ASSERT_NO_THROW(BHeapMax<int> heap);
}

TEST(DHeap, can_create_empty_heap_with_custom_comparator) {
	struct Custom {
		int number;
		int counter;
	};
	auto cmp = [](const Custom &a, const Custom &b) { return a.number < b.number; };
	using DHeapCustom = DHeap<Custom, 2, decltype(cmp)>;

	ASSERT_NO_THROW(DHeapCustom heap(cmp));
}

TEST(Binary_DHeap, range_constructor_is_correct) {
	size_t size = 10;
	std::vector<int> vec(size);

	ASSERT_NO_THROW(
		BHeapMin<int> heap(vec.begin(), vec.end());
		auto heap_vec = heap.get_vector();

		for (size_t i = 0; i < size; ++i) {
			if (2*i + 1 < size)
				EXPECT_EQ(heap_vec[i] <= heap_vec[2*i + 1], true);
			if (2*i + 2 < size)
				EXPECT_EQ(heap_vec[i] <= heap_vec[2*i + 2], true);
		}
	);
}

TEST(Binary_DHeap, vector_constructor_is_correct) {
	size_t size = 10;
	std::vector<int> vec(size);

	ASSERT_NO_THROW(
		BHeapMin<int> heap(vec);
		auto heap_vec = heap.get_vector();

		for (size_t i = 0; i < size; ++i) {
			if (2*i + 1 < size)
				EXPECT_EQ(heap_vec[i] <= heap_vec[2*i + 1], true);
			if (2*i + 2 < size)
				EXPECT_EQ(heap_vec[i] <= heap_vec[2*i + 2], true);
		}
	);
}

TEST(Binary_DHeap, push_is_correct) {
	size_t size = 100;
	std::vector<int> vec(size);
	BHeapMin<int> heap;

	for (size_t i = 0; i < size; ++i) {
		heap.push(vec[i]);
		auto heap_vec = heap.get_vector();

		for (size_t j = 0; j <= i; ++j) {
			if (2*j + 1 < i+1)
				EXPECT_EQ(heap_vec[j] <= heap_vec[2*j + 1], true);
			if (2*j + 2 < i+1)
				EXPECT_EQ(heap_vec[j] <= heap_vec[2*j + 2], true);
		}
	}
}

TEST(Binary_DHeap, front_and_pop_are_correct) {
	size_t size = 100;
	std::vector<int> vec(size);
	BHeapMin<int> heap(vec);

	sort(vec.begin(), vec.end());
	for (size_t i = 0; i < size; ++i) {
		EXPECT_EQ(heap.front(), vec[i]);
		
		heap.pop();
		auto heap_vec = heap.get_vector();
		for (size_t j = 0; j < size-i-1; ++j) {
			if (2*j + 1 < size-i-1)
				EXPECT_EQ(heap_vec[j] <= heap_vec[2*j + 1], true);
			if (2*j + 2 < size-i-1)
				EXPECT_EQ(heap_vec[j] <= heap_vec[2*j + 2], true);
		}
	}
}

template <typename Value>
using TernaryHeap = DHeap<Value,3>;

TEST(DHeap, range_constructor_is_correct) {
	size_t size = 100;
	size_t d = 3;
	std::vector<int> vec(size);

	ASSERT_NO_THROW(
		TernaryHeap<int> heap(vec.begin(), vec.end());
		auto heap_vec = heap.get_vector();

		for (size_t i = 0; i < size; ++i) {
			for (size_t j = 1; j <= d; ++j)
				if (d*i + j < size)
					EXPECT_EQ(heap_vec[i] <= heap_vec[d*i + j], true);
		}
	);
}

TEST(DHeap, vector_constructor_is_correct) {
	size_t size = 10;
	size_t d = 3;
	std::vector<int> vec(size);

	ASSERT_NO_THROW(
		TernaryHeap<int> heap(vec);
		auto heap_vec = heap.get_vector();

		for (size_t i = 0; i < size; ++i) {
			for (size_t j = 1; j <= d; ++j)
				if (d*i + j < size)
					EXPECT_EQ(heap_vec[i] <= heap_vec[d*i + j], true);
		}
	);
}

TEST(DHeap, push_is_correct) {
	size_t size = 100;
	size_t d = 3;
	std::vector<int> vec(size);
	BHeapMin<int> heap;

	for (size_t i = 0; i < size; ++i) {
		heap.push(vec[i]);
		auto heap_vec = heap.get_vector();

		for (size_t j = 0; j <= i; ++j) {
			for (size_t k = 1; k <= d; ++k)
				if (d*j + k < i+1)
					EXPECT_EQ(heap_vec[j] <= heap_vec[d*j + k], true);
		}
	}
}

TEST(DHeap, front_and_pop_are_correct) {
	size_t size = 100;
	size_t d = 3;
	std::vector<int> vec(size);
	BHeapMin<int> heap(vec);

	sort(vec.begin(), vec.end());
	for (size_t i = 0; i < size; ++i) {
		EXPECT_EQ(heap.front(), vec[i]);

		heap.pop();
		auto heap_vec = heap.get_vector();
		for (size_t j = 0; j < size-i-1; ++j) {
			for (size_t k = 1; k <= d; ++k)
				if (d*j + k < size-i-1)
					EXPECT_EQ(heap_vec[j] <= heap_vec[d*j + k], true);
		}
	}
}