#include "Heap.h"
#include "AVLTree.h"
#include "RBTree.h"
#include "Graph.h"
#include <chrono>
#include <random>
#include <string>
#include <unordered_set>
using namespace std;
using namespace chrono;

inline bool is_empty_string(string str) {
	str.erase(remove_if(str.begin(), str.end(), [](char ch) {return (ch == ' ' || ch == '\t'); }), str.end());
	return str == "";
}

inline string input() {
	string response,
		dummy;
	cin >> response;
	getline(cin, dummy);

	// Ввод всегда численный; если на вход поступает
	// не число или строка с несколькими словами,
	// выбрасывается исключение
	try {
		if (!is_empty_string(dummy))
			throw;
		stoi(response);
	}
	catch (...) {
		throw "Incorrect input, try again";
	}

	return response;
}

AdjList generate_random_graph(size_t num_vertices, size_t num_edges, int max_cost) {
	mt19937 gen(random_device{}());
	uniform_int_distribution<size_t> rand_vertex(1, num_vertices);
	uniform_int_distribution<int> rand_cost(0, max_cost);

	AdjList adj_list(num_vertices+1);
	size_t v1 = 0, v2 = 0;
	int cost;

	++num_vertices;
	// Однозначность всех рёбер обеспечивается хранением хешей рёбер,
	// который при фиксированном числе вершин определяется однозначно
	// для пары рёбер как 'v1 * num_vertices + v2' (v1, v2 - вершины графа)
	// Граф неориентированный, поэтому также хранится 'v2 * num_vertices + v1'
	unordered_set<size_t> edge_hash;
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

	return adj_list;
}

AdjList enter_user_graph(size_t num_vertices, size_t num_edges) {
	AdjList adj_list(num_vertices+1);
	string str_v1, str_v2, str_cost, dummy;
	size_t v1 = 0, v2 = 0;
	int cost;

	cout << "Enter the edges of graph\n";
	cout << "Rules:\n";
	cout << "1) Edges are entered in the format \"v1 v2 cost\" where 'v1' and 'v2' are vertices, 'cost' is edge's cost\n";
	cout << "2) 'v1', 'v2' are integers with values from [1; number of vertices], 'cost' is a positive integer\n";
	cout << "3) Graph is ordinary i.e. it's undirected and it doesn't have loops and multiple edges.\n";
	cout << "   This is means edge (v1,v2) is the same as (v2,v1), 'v1' != 'v2' and only unique edges are allowed\n";

	unordered_set<size_t> edge_hash;
	for (int i = 1; i <= num_edges; ++i) {
		for (;;) {
			cout << "Vertex " << i << ": ";
			try {
				// Ввод данных
				cin >> str_v1 >> str_v2 >> str_cost;
				getline(cin, dummy);

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
				cout << msg << endl;
			}
		}
	}

	return adj_list;
}

void enter_num_of_vertices_and_edges(int &num_vertices, int &num_edges) {
	string str_num_vertices,
	       str_num_edges;
	for (;;) {
		cout << "Enter the number of vertices: ";
		try {
			str_num_vertices = input();
			num_vertices = stoi(str_num_vertices);
			if (num_vertices <= 0)
				throw "Graph must have at least 1 vertex";

			break;
		}
		catch (const char *msg) {
			cout << msg << endl;
		}
	}
	for (;;) {
		cout << "Enter the number of edges: ";
		try {
			str_num_edges = input();
			num_edges = stoi(str_num_edges);

			if (num_edges < 0)
				throw "Graph can't have negative number of edges";
			if (num_edges > num_vertices*(num_vertices-1)/2)
				throw "Graph can't have more edges than the complete graph (edges <= vertices * (vertices-1) / 2)";

			break;
		}
		catch (const char *msg) {
			cout << msg << endl;
		}
	}
}

void apply_dijkstra(const Graph &graph) {
	string str_initial_vertex;
	size_t initial_vertex;

	for (;;) {
		cout << "Enter the initial vertex in range [" << 1 << "; " << graph.num_of_vertices() << "]: ";
		try {
			str_initial_vertex = input();
			try {
				initial_vertex = stoi(str_initial_vertex);
			}
			catch (...) {
				throw "Incorrect input, try again";
			}
			if (initial_vertex < 1 || initial_vertex > graph.num_of_vertices())
				throw "Vertex is out of bounds";

			break;
		}
		catch (const char *msg) {
			cout << msg << endl;
		}
	}

	string response;
	Graph::PathInfo result;

	for (;;) {
		cout << "\nChoose the data structure which is used in Dijkstra's algorithm:\n";
		cout << "1 - Heap\n";
		cout << "2 - AVL-tree\n";
		cout << "3 - Red-black tree\n";
		cout << "Choice: ";
		try {
			response = input();

			auto begin = steady_clock::now();
			if (response == "1")
				result = graph.dijkstra(initial_vertex, HEAP);
			else if (response == "2")
				result = graph.dijkstra(initial_vertex, AVL);
			else if (response == "3")
				result = graph.dijkstra(initial_vertex, RB);
			else
				throw "Incorrect input, try again";

			auto end = steady_clock::now();
			cout << "Running time: " << duration_cast<milliseconds>(end-begin).count() << " ms\n";
			cout << "Data structure: " << ((response == "1") ? "Heap" : (response == "2") ? "AVL-tree" : "Red-black tree") << "\n";
			break;
		}
		catch (const char *msg) {
			cout << msg << endl;
		}
	}

	for (;;) {
		cout << "\nWhat's next?\n";
		cout << "1 - Get information about shortest paths\n";
		cout << "2 - Print graph\n";
		cout << "3 - Enter another graph or apply Dijkstra's algorithm for another vertex\n";
		cout << "Choice: ";

		try {
			response = input();
			if (response == "1") {
				string str_curr_vertex;
				int curr_vertex;

				for (;;) {
					cout << "Enter the vertex in range [" << 1 << "; " << graph.num_of_vertices() << "] you want to get the path to: ";
					try {
						str_curr_vertex = input();
						try {
							curr_vertex = stoi(str_curr_vertex);
						}
						catch (...) {
							throw "Incorrect input, try again";
						}
						if (curr_vertex < 1 || curr_vertex > graph.num_of_vertices())
							throw "Vertex is out of bounds";

						cout << "Initial vertex: " << initial_vertex << '\n';
						cout << "Chosen vertex:  " << curr_vertex << '\n';
						if (result.get_path(curr_vertex).empty())
							cout << "There's no path from vertex " << initial_vertex << " to vertex " << curr_vertex << '\n';
						else {
							cout << "Cost of path:   " << result.get_cost(curr_vertex) << '\n';
							cout << "Path:           ";
							for (auto &v: result.get_path(curr_vertex))
								cout << v << "->";
							std::cout << "\b\b  \n";
						}

						break;
					}
					catch (const char *msg) {
						cout << msg << endl;
					}
				}
			}
			else if (response == "2")
				graph.print();
			else if (response == "3")
				break;
			else
				throw "Incorrect input, try again";
		}
		catch (const char *msg) {
			cout << msg << endl << endl;
		}
	}
}

inline void exit() {
	cout << "Exiting...";
	exit(0);
}

inline void start_message() {
	cout << "This is a user application for creating graphs and applying Dijkstra's algorithm to them.\n";
	cout << "You can:\n";
	cout << "    1) Create graphs manually of generate them randomly;\n";
	cout << "    2) Print graphs;\n";
	cout << "    3) Apply Dijkstra's algorithm to chosen vertex and then get the shortest path to any vertex.\n";
	cout << "Enjoy!\n";
}


void menu() {
	start_message();

	int num_vertices = 0, 
	    num_edges = 0;
	string response; // Ответ пользователя

	for (;;) {
		// Выбор основного действия
		cout << "\nWhat do you want to do?\n";
		cout << "0 - Exit\n";
		cout << "1 - Create graph\n";
		cout << "Choice: ";
		try {
			response = input();
			if (response != "0" && response != "1")
				throw "Incorrect answer, try again";
			if (response == "0")
				exit();
			else for (;;) {
				// Выбор способа ввода графа
				cout << "\nWhich way do you want to create the graph?\n";
				cout << "0 - Exit\n";
				cout << "1 - Manually\n";
				cout << "2 - Randomly\n";
				cout << "Choice: ";
				try {
					response = input();
					if (response != "0" && response != "1" && response != "2")
						throw "Incorrect answer, try again";
					if (response == "0")
						exit();
					else {
						// Ввод количества вершин и рёбер
						enter_num_of_vertices_and_edges(num_vertices, num_edges);
						Graph graph;
						// Заполнение графа (1 - пользователем, 2 - случайным образом)
						if (response == "2")
							graph = Graph(generate_random_graph(num_vertices, num_edges, 20));
						else
							graph = Graph(enter_user_graph(num_vertices, num_edges));
						cout << '\n';
						graph.print();

						for (;;) {
							// Выбор действия с графом
							cout << "\nWhat to do with the graph?\n";
							cout << "0 - Exit\n";
							cout << "1 - Print graph\n";
							cout << "2 - Apply Dijkstra algorithm\n";
							cout << "3 - Enter another graph\n";
							cout << "Choice: ";
							try {
								response = input();
								if (response == "0")
									exit();
								else if (response == "1") {
									cout << '\n';
									graph.print();
								}
								else if (response == "2")
									apply_dijkstra(graph);
								else if (response == "3")
									break;
								else
									throw "Incorrect input, try again";
							}
							catch (const char *msg) {
								cout << msg << endl;
							}
						}
					}
					break;
				}
				catch (const char *msg) {
					cout << msg << endl;
				}
			}
		}
		catch (const char *msg) {
			cout << msg << endl;
		}
	}
}

int main() {
	menu();
	return 0;
}
