#pragma once
#include "AVLTree.h"
#include "Heap.h"
#include "RBTree.h"

using vertex_type = size_t;
using cost_type = int;
using AdjList = std::vector<std::vector<std::pair<cost_type,vertex_type>>>;

enum PriorityQueueMode {HEAP, AVL, RB};

class Graph {
private:

	AdjList adj_list;
public:

	class PathInfo {
		friend class Graph;
	private:
		std::vector<std::pair<cost_type, std::vector<vertex_type>>> paths; // minimal cost + path
	public:
		PathInfo() {}
		cost_type get_cost(vertex_type i) { return paths[i].first; }
		std::vector<vertex_type> get_path(vertex_type i) { return paths[i].second; }
	};

	Graph(const AdjList &copy = AdjList()): adj_list(copy) {
		for (size_t i = 1; i < adj_list.size(); ++i) {
			for (auto &v: adj_list[i]) {
				if (v.first < 0)
					throw "Only non-negative weights of edges are allowed";
			}
		}
	}

	Graph &operator=(const Graph &copy) {
		adj_list = copy.adj_list;

		return *this;
	}

	PathInfo dijkstra(size_t start_v, PriorityQueueMode mode) const {
		std::vector<int> min_cost(adj_list.size());
		std::vector<int> prev(adj_list.size());
		std::vector<bool> was(adj_list.size());

		std::fill(min_cost.begin(), min_cost.end(), INT_MAX);
		min_cost[start_v] = 0;
		prev[start_v] = -1;

		// Выполнение алгоритма
		std::pair<cost_type, vertex_type> curr; 
		if (mode == HEAP) {
			DHeap<std::pair<cost_type, vertex_type>, 2> queue;
			queue.push({0, start_v});

			while (!queue.empty()) {
				curr = queue.front();
				queue.pop();
				if (was[curr.second])
					continue;

				was[curr.second] = true;
				for (auto &edge: adj_list[curr.second]) {
					if (curr.first + edge.first < min_cost[edge.second]) {
						min_cost[edge.second] = curr.first + edge.first;
						prev[edge.second] = curr.second;
						queue.push({min_cost[edge.second], edge.second});
					}
				}
			}
		}
		else if (mode == AVL) {
			AVLTree<std::pair<cost_type, vertex_type>> queue;
			queue.insert({0, start_v});

			while (!queue.empty()) {
				curr = *queue.begin();
				queue.erase(queue.begin());
				if (was[curr.second])
					continue;

				was[curr.second] = true;
				for (auto &edge: adj_list[curr.second]) {
					if (curr.first + edge.first < min_cost[edge.second]) {
						min_cost[edge.second] = curr.first + edge.first;
						prev[edge.second] = curr.second;
						queue.insert({min_cost[edge.second], edge.second});
					}
				}
			}
		}
		else if (mode == RB) {
			RBTree<std::pair<cost_type, vertex_type>> queue;
			queue.insert({0, start_v});

			while (!queue.empty()) {
				curr = *queue.begin();
				queue.erase(queue.begin());
				if (was[curr.second])
					continue;

				was[curr.second] = true;
				for (auto &edge: adj_list[curr.second]) {
					if (curr.first + edge.first < min_cost[edge.second]) {
						min_cost[edge.second] = curr.first + edge.first;
						prev[edge.second] = curr.second;
						queue.insert({min_cost[edge.second], edge.second});
					}
				}
			}
		}
		else
			throw "Incorrect code for priority queue data structure";

		// Запись всех кратчайших путей и их стоимостей в класс PathInfo
		PathInfo result;
		for (size_t i = 0; i < adj_list.size(); ++i) {
			result.paths.push_back({min_cost[i], std::vector<vertex_type>()});
			if (min_cost[i] != INT_MAX) {
				for (int curr_v = i; curr_v != -1; curr_v = prev[curr_v])
					result.paths[i].second.push_back(curr_v);
				std::reverse(result.paths[i].second.begin(), result.paths[i].second.end());
			}
		}

		// Вывод всех кратчайших путей
		size_t count = 0;
		std::cout << "All shortest paths" << std::endl;
		std::cout << "Initial vertex: " << start_v << std::endl;
		for (size_t i = 1; i < adj_list.size(); ++i) {
			if (result.paths[i].second.empty())
				std::cout << "Vertex " << i << "- inaccessible from vertex " << start_v << '\n';
			else {
				std::cout << "Vertex " << i << " - minimal cost: " << result.paths[i].first << ", ";
				std::cout << "shortest path: ";
				for (auto &vertex: result.paths[i].second)
					std::cout << vertex << "->";
				std::cout << "\b\b  \n";
			}
		}

		return result;
	}

	size_t num_of_vertices() const {
		return (adj_list.size() - 1); 
	}

	void print() const {
		std::cout << "\nVertices and their neighbours (vertex, cost):" << std::endl;

		for (size_t i = 1; i < adj_list.size(); ++i) {
			std::cout << "Vertex " << i << ": ";
			for (auto &edge: adj_list[i])
				std::cout << '(' << edge.second << ',' << edge.first << "), ";
			std::cout << "\b\b  \n";
		}
	}
};