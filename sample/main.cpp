#pragma once
#include "Heap.h"
#include "AVLTree.h"
#include "RBTree.h"
#include "Graph.h"
#include <random>
#include <iostream>
#include <numeric>
using namespace std;
const int sz = 10;
static mt19937 gen(random_device{}());

int main() {
	
	/*
	std::vector<int> sample(sz), ans;
	std::mt19937 gen(std::random_device{}());
	for (size_t i = 0; i < sz; ++i)
		sample[i] = gen();

	size_t n, m, ai, aj;
	int w;
	cin >> n >> m;
	AdjList adj(n);
	for (int i = 0; i < m; ++i) {
		cin >> ai >> aj >> w;
		adj[ai].push_back({w,aj});
		adj[aj].push_back({w,ai});
	}

	Graph graph(adj);
	graph.dijkstra(0, AVL);
	graph.dijkstra(1, RB);
	graph.dijkstra(2, HEAP);
	*/
	int attempt = 0;
	for (; attempt < 10000; ++attempt) {
		size_t size = 100;
		vector<int> vec(size);
		iota(vec.begin(), vec.end(), 1);
		shuffle(vec.begin(), vec.end(), gen);

		/*
		cout << "Insert vector: ";
		for (auto elem: vec)
			cout << elem << ',';
		cout << endl;
		*/
		AVLTree<int> tree(vec.begin(), vec.end());

		sort(vec.begin(), vec.end());
		auto vec_it = vec.begin();
		auto tree_it = tree.begin();
		for (size_t i = 0; i < size; ++i, ++vec_it, ++tree_it)
			if (*vec_it != *tree_it) {
				//cout << "YOUR TREE IS GARBO!!!" << endl;
				return -1;
			}
		cout << "Insertion is fine ^_^" << endl;

		shuffle(vec.begin(), vec.end(), gen);
		//vec = {7,4,5,8,2,1,6,3};
		/*
		cout << "Erase vector: ";
		for (auto elem: vec)
			cout << elem << ',';
		cout << endl;
		*/

		for (auto &elem: vec) {
			tree.erase(elem);
			if (tree.find(elem) != tree.end()) {
				//cout << "YOUR DELETION IS GARBO!!!" << endl;
				return -2;
			}
		}
		cout << "Deletion is fine ^_^" << endl;
	}
	cout << attempt;

	return 0;
}