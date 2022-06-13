#include "../../../include/AVLTree.h"

using namespace std;

int AVLTree::Diff(Node* temp) {
	if (temp == nullptr)
		return 0;
	int res;

	if ((temp->left == nullptr) && (temp->right == nullptr))
		return 0;
	if ((temp->left != nullptr) && (temp->right != nullptr))
		res = temp->left->h - temp->right->h;
	if ((temp->left != nullptr) && (temp->right == nullptr))
		res = (temp->left->h) + 1;
	if ((temp->left == nullptr) && (temp->right != nullptr))
		res = -((temp->right->h) + 1);

	return res;
}

void AVLTree::Turn_little_R(Node* node) {
	Node* tmp;
	tmp = node->left;
	node->left = tmp->right;
	if (node->left != nullptr)
		node->left->parent = node;
	tmp->right = node;
	tmp->parent = node->parent;
	node->parent = tmp;
	if (tmp->parent != nullptr) {
		if (tmp->parent->left == node)
			tmp->parent->left = tmp;
		else
			tmp->parent->right = tmp;
	}
	else 
		ROOT = tmp;

	if ((tmp->left == nullptr) && (tmp->right == nullptr))
		tmp->h = 0;
	if ((tmp->left != nullptr) && (tmp->right == nullptr))
		tmp->h = tmp->left->h + 1;
	if ((tmp->left == nullptr) && (tmp->right != nullptr))
		tmp->h = tmp->right->h + 1;
	if ((tmp->left != nullptr) && (tmp->right != nullptr))
		tmp->h = max(tmp->left->h, tmp->right->h) + 1;
	
	if ((tmp->right->left == nullptr) && (tmp->right->right == nullptr))
		tmp->right->h = 0;
	if ((tmp->right->left != nullptr) && (tmp->right->right == nullptr))
		tmp->right->h = tmp->right->left->h + 1;
	if ((tmp->right->left == nullptr) && (tmp->right->right != nullptr))
		tmp->right->h = tmp->right->right->h + 1;
	if ((tmp->right->left != nullptr) && (tmp->right->right != nullptr))
		tmp->right->h = max(tmp->right->left->h, tmp->right->right->h) + 1;
}

void AVLTree::Turn_little_L(Node* node) {
	Node* tmp;
	tmp = node->right;
	node->right = tmp->left;
	if (node->right != nullptr)
		node->right->parent = node;
	tmp->left = node;
	tmp->parent = node->parent;
	node->parent = tmp;
	if (tmp->parent != nullptr)
	{
		if (tmp->parent->left == node)
			tmp->parent->left = tmp;
		else
			tmp->parent->right = tmp;
	}
	else ROOT = tmp;
	
	if ((tmp->left == nullptr) && (tmp->right == nullptr))
		tmp->h = 0;
	if ((tmp->left != nullptr) && (tmp->right == nullptr))
		tmp->h = tmp->left->h + 1;
	if ((tmp->left == nullptr) && (tmp->right != nullptr))
		tmp->h = tmp->right->h + 1;
	if ((tmp->left != nullptr) && (tmp->right != nullptr))
		tmp->h = max(tmp->left->h, tmp->right->h) + 1;
	
	if ((tmp->left->left == nullptr) && (tmp->left->right == nullptr))
		tmp->left->h = 0;
	if ((tmp->left->left != nullptr) && (tmp->left->right == nullptr))
		tmp->left->h = tmp->left->h + 1;
	if ((tmp->left->left == nullptr) && (tmp->left->right != nullptr))
		tmp->left->h = tmp->left->right->h + 1;
	if ((tmp->left->left != nullptr) && (tmp->left->right != nullptr))
		tmp->left->h = max(tmp->left->left->h, tmp->left->right->h) + 1;
}

void AVLTree::Turn_big_L(Node* node) {
	Turn_little_R(node->right);
	Turn_little_R(node);
}

void AVLTree::Turn_big_R(Node* node) {
	Turn_little_L(node->left);
	Turn_little_R(node);
}

void AVLTree::Balance(Node* node) {
	Node* tmp;
	int d;
	int d1, d2;
	for (tmp = node; tmp != nullptr; tmp = tmp->parent)
	{
		if ((tmp->left == nullptr) && (tmp->right == nullptr))
			tmp->h = 0;
		if ((tmp->left != nullptr) && (tmp->right == nullptr))
			tmp->h = tmp->left->h + 1;
		if ((tmp->left == nullptr) && (tmp->right != nullptr))
			tmp->h = tmp->right->h + 1;
		if ((tmp->left != nullptr) && (tmp->right != nullptr))
			tmp->h = max(tmp->left->h, tmp->right->h) + 1; 
		d = Diff(tmp);
		d1 = Diff(tmp->left);
		d2 = Diff(tmp->right);

		if (d == -2)
		{
			if (d2 == 1)
				Turn_big_L(tmp);
			if (d2 <= 0) 
				Turn_little_L(tmp);
		}

		if (d == 2)
		{
			if (d1 == -1)
				Turn_big_R(tmp);
	
			if (d1 >= 0)
				Turn_little_R(tmp);
		}
	}
}

void AVLTree::Insert(int priority, int num) {
	Node* temp = ROOT;
	Node* _temp = ROOT;
	Node* temp1;
	Node* A = new Node();
	A->left = nullptr;
	A->right = nullptr;
	A->data.first = priority;
	A->data.second = num;
	A->h = 0;
	if (size != 0)
	{
		while (temp != nullptr)
		{
			_temp = temp;
			if (priority < temp->data.first)
				temp = temp->left;
			else temp = temp->right;
		}
		A->parent = _temp;
		if (priority < _temp->data.first)
			_temp->left = A;

		if (priority > _temp->data.first)
			_temp->right = A;
	}
	else
	{
		A->parent = nullptr;
		this->ROOT = A;
	}
	this->size += 1;
	for (temp1 = A; temp1 != nullptr; temp1 = temp1->parent)
		Balance(temp1);
}

Node* AVLTree::Search(int num) {
	Node* temp = ROOT;
	Node* answer = nullptr;
	while (temp != nullptr)
	{
		if (num == temp->data.first)
		{
			answer = temp;
			break;
		}
		if (num < temp->data.first)
			temp = temp->left;
		else
			temp = temp->right;
	}
	return answer;
}

Node* AVLTree::Max(Node* node) {
	Node* temp = node;
	Node* _temp = temp;
	while (temp != nullptr)
	{
		_temp = temp;
		temp = temp->right;
	}
	return _temp;
}

Node* AVLTree::Min(Node* node) {
	Node* temp = node;
	Node* _temp = temp;
	while (temp != nullptr)
	{
		_temp = temp;
		temp = temp->left;
	}
	return _temp;
}

void AVLTree::Delete(int num) {
	Node* temp = Search(num);
	Node* temp1;
	if (temp != nullptr) {
		if ((temp->left == nullptr) && (temp->right == nullptr) && (temp != ROOT)) {
			if (temp->parent->left == temp)
				temp->parent->left = nullptr;
			else
				temp->parent->right = nullptr;

			for (temp1 = temp->parent; temp1 != nullptr; temp1 = temp1->parent)
				Balance(temp1);
		}

		if ((temp->left == nullptr) && (temp->right == nullptr) && (temp == ROOT)) {
			ROOT = nullptr;
			Balance(ROOT);
		}

		if ((temp->left == nullptr) && (temp->right != nullptr) && (temp != ROOT)) {
			if (temp->parent->left == temp) {
				temp->parent->left = temp->right;
				temp->right->parent = temp->parent;
			}
			else {
				temp->parent->right = temp->right;
				temp->right->parent = temp->parent;
			}
			for (temp1 = temp->parent; temp1 != nullptr; temp1 = temp1->parent)
				Balance(temp1);
			
		}

		if ((temp->left == nullptr) && (temp->right != nullptr) && (temp == ROOT)) {
			ROOT = temp->right;
			temp->right->parent = nullptr;
			Balance(ROOT);
		}

		if ((temp->left != nullptr) && (temp->right == nullptr) && (temp != ROOT)) {
			if (temp->parent->left == temp) {
				temp->parent->left = temp->left;
				temp->left->parent = temp->parent;
			}
			else {
				temp->parent->right = temp->left;
				temp->left->parent = temp->parent;
			}

			for (temp1 = temp->parent; temp1 != nullptr; temp1 = temp1->parent)
				Balance(temp1);
		}

		if ((temp->left != nullptr) && (temp->right == nullptr) && (temp == ROOT)) {
			ROOT = temp->left;
			temp->left->parent = nullptr;
			Balance(ROOT);
		}

		if ((temp->left != nullptr) && (temp->right != nullptr)) {
			Node* maxNode = Max(temp->left);
			temp->data.first = maxNode->data.first;
			temp->data.second = maxNode->data.second;
			if (maxNode->left != nullptr)
				maxNode->left->parent = maxNode->parent;
			if (maxNode->parent->left == maxNode)
				maxNode->parent->left = maxNode->left;
			else
				maxNode->parent->right = maxNode->left;
			for (temp1 = maxNode->parent; temp1 != nullptr; temp1 = temp1->parent)
				Balance(temp1);
		}
		size--;
	}
}
