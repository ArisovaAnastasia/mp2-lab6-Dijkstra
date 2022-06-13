#include "Heap.h"
#include "AVLTree.h"
#include "RBTree.h"
#include "Graph.h"
#include <chrono>
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
							graph = generate_random_graph(num_vertices, num_edges, 20);
						else
							graph = enter_user_graph(num_vertices, num_edges);
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
								else if (response == "1")
									graph.print();
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