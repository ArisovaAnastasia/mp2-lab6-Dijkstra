#include "../sln/vc12/arithmetic/Algorithm.h"

#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

int main()
{
	srand(15);
	int k = 1, l = 1;
	do {
		cout << '\n' << "Choose one of the items:" << '\n';
		string options[3] = { "1. To generate a graph","2. To input a graph","3. Exit" };
		for (int i = 0; i < 3; i++)
			cout << options[i] << '\n';
		cout << endl << "Input a menu number:";
		cin >> k;
		if ((k != 1) && (k != 2)) 
			break;
		else {
			int number, start, _node = -1;
			cout << endl << "Input a number of nodes: ";
			cin >> number;
			cout << endl << "Input a start node: ";
			cin >> start;
			Algorithm A;
			if (k == 1)
				A.Generate(number, start);
			else
				A.Form(number, start);
			if (A.Check(k)) {
				do {
					cout << '\n' << "Choose one of the items:" << '\n';
					string choices[2] = { "1. A new node","2. A new algorithm" };
					for (int i = 0; i < 2; i++)
						cout << choices[i] << '\n';
					cout << "Input a menu number:";
					cin >> l;
					if (l != 1) 
						break;
					else {
						_node = -1;
						while ((_node < 0) || (_node >= number)) {
							cout << endl << "Input a node (0<=node<number): ";
							cin >> _node;
						}
						A.Print();
						A.AVL();
						A.Way_Cost(_node);
						A.RB();
						A.Way_Cost(_node);
						A.HEAP();
						A.Way_Cost(_node);


					}
				} while (l == 1);
			}
			else
				cout << endl << "It isn't a simply connected graph";
		}
		
	} while ((k == 1) || (k == 2));
	return 0;
}