#pragma once
#include "AVLTree.h"
#include "Heap.h"
#include "RBTree.h"
#include <unordered_set>
#include <random>
#include <string>

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

inline bool is_empty_string(std::string s);

Graph generate_random_graph(size_t num_vertices, size_t num_edges, int max_cost) {
	std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<size_t> rand_vertex(1, num_vertices);
	std::uniform_int_distribution<int> rand_cost(1, max_cost);

	AdjList adj_list(num_vertices+1);
	size_t v1 = 0, v2 = 0;
	int cost;

	++num_vertices;
	// Однозначность всех рёбер обеспечивается хранением хешей рёбер,
	// который при фиксированном числе вершин определяется однозначно
	// для пары рёбер как 'v1 * num_vertices + v2' (v1, v2 - вершины графа)
	// Граф неориентированный, поэтому также хранится 'v2 * num_vertices + v1'
	std::unordered_set<size_t> edge_hash;
	for (int i = 0; i < num_edges; ++i) {
		// Ребро корректно, если вершины не совпадают и такое ребро ещё не существует
		while (!(v1 != v2 && edge_hash.find(v1*num_vertices + v2) == edge_hash.end()
			   && edge_hash.find(v2*num_vertices + v1) == edge_hash.end())) {
			v1 = rand_vertex(gen);
			v2 = rand_vertex(gen);
		}
		edge_hash.insert(v1*num_vertices + v2);
		edge_hash.insert(v2*num_vertices + v1);
		cost = rand_cost(gen);
		adj_list[v1].push_back({cost, v2});
		adj_list[v2].push_back({cost, v1});
	}

	return Graph(adj_list);
}

Graph enter_user_graph(size_t num_vertices, size_t num_edges) {
	AdjList adj_list(num_vertices+1);
	std::string str_v1, str_v2, str_cost, dummy;
	size_t v1 = 0, v2 = 0;
	int cost;

	std::cout << "\nEnter the edges of graph\n";
	std::cout << "Rules:\n";
	std::cout << "1) Edges are entered in the format \"v1 v2 cost\" where 'v1' and 'v2' are vertices, 'cost' is edge's cost\n";
	std::cout << "2) 'v1', 'v2' are integers with values from [1; number of vertices], 'cost' is a positive integer\n";
	std::cout << "3) Graph is ordinary i.e. it's undirected and it doesn't have loops and multiple edges.\n";
	std::cout << "   This is means edge (v1,v2) is the same as (v2,v1), 'v1' != 'v2' and only unique edges are allowed\n";

	std::unordered_set<size_t> edge_hash;
	for (int i = 1; i <= num_edges; ++i) {
		for (;;) {
			std::cout << "Edge " << i << ": ";
			try {
				// Ввод данных
				std::cin >> str_v1 >> str_v2 >> str_cost;
				std::getline(std::cin, dummy);

				// Обработка некорректного ввода
				if (!is_empty_string(dummy))
					throw "Incorrect input, try again";
				try {
					v1 = stoi(str_v1);
					v2 = stoi(str_v2);
					cost = stoi(str_cost);
				}
				catch (...) {
					throw "Incorrect input, try again";
				}
				if (v1 < 1 || v2 < 1 || v1 > num_vertices || v2 > num_vertices)
					throw "Vertex is out of bounds";
				if (cost < 0)
					throw "Cost of edge must be non-negative";
				if (v1 == v2)
					throw "Loops are not allowed";
				if (edge_hash.find(v1*(num_vertices + 1) + v2) != edge_hash.end() ||
					edge_hash.find(v2*(num_vertices + 1) + v1) != edge_hash.end())
					throw "Multiple edges are not allowed";

				// Вставка ребра в списки смежности
				edge_hash.insert(v1*(num_vertices + 1) + v2);
				edge_hash.insert(v2*(num_vertices + 1) + v1);
				adj_list[v1].push_back({cost, v2});
				adj_list[v2].push_back({cost, v1});
				break;
			}
			catch (const char *msg) {
				std::cout << msg << std::endl;
			}
		}
	}

	return Graph(adj_list);
}