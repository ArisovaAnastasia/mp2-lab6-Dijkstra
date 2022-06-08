#pragma once
#include <algorithm>
#include <utility>

template <typename Value, class Pred = std::less<Value>>
class AVLTree {

	struct Node {
		Node *left, 
			*right,
			*parent;
		Value data;
		int8_t bf;

		Node(const Node &copy): data(copy.data) {}
		Node(const Value &value = Value(), Node *parent = nullptr, Node *left = nullptr, Node *right = nullptr): 
			data(value), left(left), right(right), parent(parent), bf(0) {}
	};

	Pred cmp; // объект для сравнения данных на '<'
	Node *root = nullptr;
	Node *head = new Node(Value());
	size_t _size = 0;
	

public:

	// ITERATOR

	class iterator {
		friend class AVLTree;

		using value_type = Value;
		using pointer = Value*;
		using reference = Value&;
		using difference_type = ptrdiff_t;
		using iterator_category = std::bidirectional_iterator_tag;

		Node *node; // Текущая вершина
		Node *prev; // Предыдущая вершина
		Node *head; // Голова списка, по которому проходит итератор

		iterator(Node *head, Node *prev, Node *node): head(head), prev(prev), node(node) {}

	public:
		iterator(const iterator &copy): head(copy.head), node(copy.node), prev(copy.prev) {}

		reference operator*() { return node->data; }
		pointer operator->() { return &(node->data); }

		iterator& operator++() {
			if (node->right != nullptr) {
				prev = node;
				node = node->right;

				while (node->left != nullptr) {
					prev = node;
					node = node->left;
				}
			}
			else {
				if (prev != head && prev->right != node) {
					node = prev;
					prev = prev->parent;
				}
				else {
					while (prev != head && prev->right == node) {
						node = prev;
						prev = prev->parent;
					}

					if (prev == head)
						node = nullptr;
					else {
						node = prev;
						prev = prev->parent;
					}
				}
			}

			return *this;
		}
		iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

		iterator& operator--() {
			if (node->left != nullptr) {
				prev = node;
				node = node->left;

				while (node->right != nullptr) {
					prev = node;
					node = node->right;
				}
			}
			else {
				if (prev != head && prev->left != node) {
					node = prev;
					prev = prev->parent;
				}
				else {
					while (prev != head && prev->left == node) {
						node = prev;
						prev = prev->parent;
					}

					if (prev == head)
						node = nullptr;
					else {
						node = prev;
						prev = prev->parent;
					}
				}
			}

			return *this;
		}
		iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

		iterator& operator=(const iterator &copy) {
			node = copy.node;
			prev = copy.prev;
			head = copy.head;

			return *this;
		}

		inline bool operator==(const iterator &it) const {
			return (head == it.head) && (node == it.node);
		}
		inline bool operator!=(const iterator &it) const {
			return !(head == it.head) || !(node == it.node);
		}
	};

	iterator begin() const {
		Node *curr = root,
			 *prev = head;
		while (curr->left != nullptr) {
			prev = curr;
			curr = curr->left;
		}

		return iterator(head, prev, curr);
	}

	iterator end() const {
		return iterator(head, nullptr, nullptr);
	}


	// COMMON PART

	AVLTree(const Pred &cmp = Pred()): cmp(cmp) {}

	template <typename Iterator>
	AVLTree(const Iterator &first, const Iterator &last, const Pred &cmp = Pred()): cmp(cmp) {
		for (auto it = first; it != last; ++it) {
			insert(*it);
			++_size;
		}
	}

	AVLTree(const AVLTree &copy): _size(copy._size), cmp(copy.cmp) {
		_copy_tree(head, copy.head);
		root = head->left;
	}

	AVLTree(AVLTree &&mov): _size(mov._size), cmp(std::move(mov.cmp)) {
		root = head->left = mov.head->left;
		head->left->parent = head;
		mov.head->left = nullptr;
	}

	~AVLTree() {
		clear();
	}

	AVLTree& operator=(const AVLTree &copy) {
		if (this != &copy) {
			_size = copy._size;
			cmp = copy.cmp;

			if (root != nullptr)
				_delete_tree(root);
			_copy_tree(head, copy.head);
			root = head->left;
		}
		return *this;
	}

	AVLTree& operator=(AVLTree &&mov) {
		_size = mov._size;
		cmp = std::move(mov.cmp);

		root = head->left = mov.head->left;
		head->left->parent = head;
		if (head != mov.head)
			mov.head->left = nullptr;

		return *this;
	}

	size_t size() { return _size; }
	bool empty() { return _size == 0; }
	void clear() {
		if (root != nullptr)
			_delete_tree(root);
		_size = 0;
	}


	iterator find(const Value &val) const {
		Node *curr = root;
		Node *prev = head;

		while (curr != nullptr) {
			if (cmp(curr->data, val)) {
				prev = curr;
				curr = curr->right;
			}
			else if (cmp(val, curr->data)) {
				prev = curr;
				curr = curr->left;
			}
			else break;
		}

		return iterator(head, prev, curr);
	}

	iterator insert(const Value &val) {
		// Проверка наличия вершины в дереве
		iterator it = find(val);
		if (it.node != nullptr)
			throw "Element is already inserted";

		// Вставка новой вершины
		++_size;

		if (root == nullptr) { // Дерево пустое
			head->left = root = new Node(val, head);
			return iterator(head, head, root);
		}
		
		Node *curr = it.prev;
		Node *next;
		Node *return_node;
		if (cmp(val, curr->data))
			next = curr->left = new Node(val, curr);
		else
			next = curr->right = new Node(val, curr);
		return_node = next;

		// Балансировка
		while (curr != head) {
			// Меняем баланс вершины, в которую входим
			(curr->left == next) ? --curr->bf : ++curr->bf;
			// Если в какой-то момент баланс стал равен 0, высота поддерева не изменилась =>
			// => выше высота и, следовательно, баланс тоже не изменятся
			if (curr->bf == 0)
				break;
			// Если произведена балансировка, высота поддерева останется той же, что до вставки =>
			// => выше ничто не изменится
			if (curr->bf == 2 || curr->bf == -2) {
				_balance(curr);
				break;
			}
			
			next = curr;
			curr = curr->parent;
		}

		return iterator(head, return_node->parent, return_node);
	}

	void erase(iterator it) {
		--_size;

		Node *curr = it.node;
		Node *prev = it.prev;
		Node *prev_parent;
		Node *erase_node;

		// Если оба поддерева непусты, заменяем значение в текущей вершине
		// на следующее по величине и удаляем вершину, в которой оно находилось
		// (Сводим задачу к удалению листовой вершины или с 1 непустым поддеревом)
		if (curr->left != nullptr && curr->right != nullptr) {
			++it;
			curr->data = std::move(it.node->data);
			curr = it.node;
			prev = it.prev;
		}
		// Записываем удаляемую вершину в отдельную переменную, т.к. 

		erase_node = curr;
		if (curr->left != nullptr && curr->right == nullptr) { // Если удаляем вершину без правого поддерева, она заменяется левым поддеревом
			if (prev->left == curr)
				prev->left = curr->left;
			else
				prev->right = curr->left;

			curr->left->parent = prev;
			curr = curr->left;
		}
		else if (curr->left == nullptr && curr->right != nullptr) { // Если если удаляем вершину без левого поддерева, она заменяется правым поддеревом
			if (prev->left == curr)
				prev->left = curr->right;
			else
				prev->right = curr->right;

			curr->right->parent = prev;
			curr = curr->right;
		}

		// Балансировка
		prev_parent = prev->parent;
		while (prev != head) {
			bool is_prev_left = (prev_parent->left == prev) ? true : false; // признак того, с какой стороны находится 'prev'

			// Меняем баланс, чтобы уменьшилась высота того поддерева, из которого поднимаемся
			(prev->left == curr) ? ++prev->bf : --prev->bf;
			// Если баланс равен 1 или -1, до этого он был равен 0 =>
			// высота поддерева с корнем 'prev' не изменилась, балансировка завершена
			if (prev->bf == 1 || prev->bf == -1)
				break;
			// Балансируем вершину, если баланс равен 2 или -2
			if (prev->bf == 2 || prev->bf == -2) {
				_balance(prev); // здесь "ломается" положение 'prev'
				prev = (is_prev_left) ? prev_parent->left : prev_parent->right; // восстанавливаем положение 'prev'
				// Если он стал равен 1 или -1, высота осталась той же =>
				// выше баланс не изменится, балансировка завершена
				if (prev->bf == 1 || prev->bf == -1)
					break;
			}

			curr = prev;
			prev = prev->parent;
			prev_parent = prev->parent;
		}

		// Удаление вершины 'erase_node'
		if (erase_node->parent->left == erase_node)
			erase_node->parent->left = nullptr;
		else if (erase_node->parent->right == erase_node)
			erase_node->parent->right = nullptr;
		delete erase_node;

		root = head->left;
	}

	void erase(const Value &val) {
		auto it = find(val);
		if (it.node == nullptr)
			throw "Element isn't in here";

		erase(it);
	}

private:
	// Вращение налево
	void _rotate_left(Node *curr) {
		Node *tmp = curr->right;

		if (tmp->left != nullptr)
			tmp->left->parent = curr;
		curr->right = tmp->left;

		tmp->parent = curr->parent;
		if (tmp->parent->left == curr)
			tmp->parent->left = tmp;
		else
			tmp->parent->right = tmp;

		curr->parent = tmp;
		tmp->left = curr;

		if (curr == root)
			root = tmp;

		curr->bf = curr->bf - 1 - std::max((int8_t)0, tmp->bf);
		tmp->bf = tmp->bf - 1 + std::min((int8_t)0, curr->bf);

		curr = tmp;
	}

	// Вращение направо
	void _rotate_right(Node *curr) {
		Node *tmp = curr->left;

		if (tmp->right != nullptr)
			tmp->right->parent = curr;
		curr->left = tmp->right;

		tmp->parent = curr->parent;
		if (tmp->parent->left == curr)
			tmp->parent->left = tmp;
		else
			tmp->parent->right = tmp;

		curr->parent = tmp;
		tmp->right = curr;

		if (curr == root)
			root = tmp;

		curr->bf = curr->bf + 1 - std::min((int8_t)0, tmp->bf);
		tmp->bf = tmp->bf + 1 + std::max((int8_t)0, curr->bf);

		curr = tmp;
	}

	// Балансировка
	void _balance(Node *curr) {
		if (curr->bf == 2) {
			if (curr->right->bf >= 0)
				_rotate_left(curr);
			else {
				_rotate_right(curr->right);
				_rotate_left(curr);
			}
		}
		else if (curr->bf == -2){
			if (curr->left->bf <= 0)
				_rotate_right(curr);
			else {
				_rotate_left(curr->left);
				_rotate_right(curr);
			}
		}
	}

	// Копирование дерева
	void _copy_tree(Node *old_tree, Node *new_tree) {
		if (old_tree->left == nullptr && old_tree->right == nullptr)
			return;
		if (old_tree->left != nullptr) {
			new_tree->left = new Node(old_tree->left->data, new_tree);
			_copy_tree(old_tree->left, new_tree->left);
		}
		if (old_tree->right != nullptr) {
			new_tree->right = new Node(old_tree->right->data, new_tree);
			_copy_tree(old_tree->right, new_tree->right);
		}
	}

	// Удаление дерева
	void _delete_tree(Node *tree) {
		if (tree->left != nullptr)
			_delete_tree(tree->left);
		if (tree->right != nullptr)
			_delete_tree(tree->right);
		delete tree;
	}
};