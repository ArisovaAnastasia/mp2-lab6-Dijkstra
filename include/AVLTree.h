#ifndef _AVLTREE_H
#define _AVLTREE_H

#include "iostream"
#include <algorithm>

using namespace std;

struct Node {
	int h;
	pair<int, int> data; // data.first-priority, data.second - Node1's number //
	Node* left;
	Node* right;
	Node* parent;
};

class AVLTree {
private:
	Node* ROOT;
	int size;
public:
	AVLTree() : ROOT(nullptr), size(0) {}
	Node* Root() { return ROOT; }
	int High() { return ROOT->h; }
	int Diff(Node* temp);
	void Turn_little_R(Node* node);
	void Turn_little_L(Node *node);
	void Turn_big_L(Node* node);
	void Turn_big_R(Node* node);
	void Balance(Node* node);
	void Insert(int priority, int number);
	Node* Search(int num);
	Node* Max(Node* node);
	Node* Min(Node* node);
	void Delete(int num);
};

#endif _AVLTREE_H