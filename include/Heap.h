#pragma once
#include <vector>
#include <utility>
#include <iostream>
#include <cstdint>

template <typename Value, int64_t d, class Pred = std::less<Value>>
class DHeap {
private:
	// Проверка параметра на адекватность
	static_assert(d > 1, "D-heap with D < 2 is not allowed");

	// ^^^ Поля класса vvv
	std::vector<Value> arr = std::vector<Value>();
	Pred cmp;
	// ^^^ Поля класса ^^^

	
	// "Просеивание" элемента вверх
	inline void _sift_up(int64_t i) {
		while (i > 0 && cmp(arr[i],arr[(i-1)/d])) {
			std::swap(arr[i], arr[(i-1)/d]);
			i = (i-1)/d;
		}
	}

	// "Просеивание" элемента вниз
	inline void _sift_down(int64_t i) {
		int64_t size = arr.size();
		int64_t min_idx, end_idx;

		while (i*d + 1 < size) {
			min_idx = i;
			end_idx = std::min(i*d + d, size);

			for (int curr_idx = i*d; curr_idx <= end_idx; ++curr_idx)
				if (cmp(arr[curr_idx],arr[min_idx]))
					min_idx = curr_idx;

			if (min_idx == i) break;
			std::swap(arr[min_idx], arr[i]);
			i = min_idx;
		}
	}

public:

	DHeap(): cmp(Pred()) {}

	template<typename Iterator>
	DHeap(const Iterator &first, const Iterator &second, const Pred &_cmp = Pred()): cmp(_cmp) {
		for (auto it = first; it != second; ++it)
			push(*it);
	}

	DHeap(const std::vector<Value> &vec, const Pred &_cmp = Pred()): arr(vec), cmp(_cmp) {
		int64_t bound = (arr.size()-2+d)/d;

		for (int64_t i = bound-1; i >= 0; --i)
			_sift_down(i);
	}

	DHeap(const DHeap &copy): arr(copy.arr), cmp(copy.cmp) {}
	DHeap(DHeap &&mov): arr(std::move(mov.arr)), cmp(std::move(mov.cmp)) {}

	DHeap& operator=(const DHeap &copy) {
		arr = copy.arr;
		cmp = copy.cmp;

		return *this;
	}

	DHeap &operator=(DHeap &&mov) {
		arr = std::move(mov.arr);
		cmp = std::move(mov.cmp);

		return *this;
	}

	void push(const Value &value) {
		arr.push_back(value);
		_sift_up(arr.size() - 1);
	}

	void pop() {
		std::swap(arr[0], arr.back());
		arr.pop_back();
		_sift_down(0);
	}

	Value front() {
		return arr[0];
	}

	std::vector<Value> get_vector() { return arr; }
	int64_t size() { return arr.size(); }
	bool empty() { return arr.empty(); }
	void clear() { arr.clear(); }
};

template <typename Value, typename Pred>
class DHeap<Value, 2, Pred> {
private:

	std::vector<Value> arr = std::vector<Value>();
	Pred cmp;

	// "Просеивание" элемента вверх
	inline void _sift_up(int64_t i) {
		while (i > 0 && cmp(arr[i],arr[(i-1)>>1])) {
			std::swap(arr[i], arr[(i-1)>>1]);
			i = (i-1)>>1;
		}
	}

	// "Просеивание" элемента вниз
	inline void _sift_down(int64_t i) {
		int64_t size = arr.size();
		int64_t j, left, right;

		while ((i<<1) + 1 < size) {
			j = i;
			left  = (i<<1) + 1;
			right = (i<<1) + 2;

			if (left < size && cmp(arr[left],arr[j]))
				j = left;
			if (right < size && cmp(arr[right],arr[j]))
				j = right;
			if (j == i) break;

			std::swap(arr[j], arr[i]);
			i = j;
		}
	}

public:

	DHeap(const Pred &cmp = Pred()): cmp(cmp) {}

	template<typename Iterator>
	DHeap(const Iterator &first, const Iterator &second, const Pred &_cmp = Pred()): cmp(_cmp) {

		for (auto it = first; it != second; ++it)
			push(*it);
	}

	DHeap(const std::vector<Value> &vec, const Pred &_cmp = Pred()): arr(vec), cmp(_cmp) {
		int64_t bound = arr.size()>>1;

		for (int64_t i = bound-1; i >= 0; --i)
			_sift_down(i);

	}

	DHeap(const DHeap &copy): arr(copy.arr), cmp(copy.cmp) {}
	DHeap(DHeap &&mov): arr(std::move(mov.arr)), cmp(mov.cmp) {}

	void push(const Value &value) {
		arr.push_back(value);
		_sift_up(arr.size() - 1);
	}

	void pop() {
		std::swap(arr[0], arr.back());
		arr.pop_back();
		_sift_down(0);
	}

	Value front() {
		return arr[0];
	}

	std::vector<Value> get_vector() { return arr; }
	int64_t size() { return arr.size(); }
	bool empty() { return arr.empty(); }
	void clear() { arr.clear(); }
};