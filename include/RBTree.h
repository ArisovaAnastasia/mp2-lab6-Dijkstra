#pragma once
#include <algorithm>
#include <utility>

template <typename Value, class Pred = std::less<Value>>
class RBTree {

	enum Colour: int8_t {Black, Red}; // Цвет вершины

	struct Node {
		Node  *left, 
			  *right,
			  *parent;
		Value  data;
		Colour colour;

		Node(const Node &copy): data(copy.data) {}
		Node(const Value &value = Value(), 
			       Colour col = Black, 
			       Node *left = nullptr, 
			       Node *right = nullptr, 
			       Node *parent = nullptr): 
			data(value), left(left), right(right), parent(parent), colour(col) {}
	};

	Pred cmp; // компаратор для сравнения элементов отношением '<'
	Node *nil = new Node(Value(), Black); // "Фиктивная" листовая вершина и корень дерева
	Node *root;
	size_t _size = 0;

public:

	// ITERATOR

	class iterator {
		friend class RBTree;

		using value_type = Value;
		using pointer = Value*;
		using reference = Value&;
		using difference_type = ptrdiff_t;
		using iterator_category = std::bidirectional_iterator_tag;

		Node *node;
		Node *prev;
		Node *nil;

		iterator(Node *node, Node *prev, Node *nil): node(node), prev(prev), nil(nil) {}

	public:

		iterator(const iterator &copy): node(copy.node), prev(copy.prev), nil(copy.nil) {}

		reference operator*() { return node->data; }
		pointer operator->() { return &(node->data); }

		iterator& operator++() {
			// Если правое поддерево непусто, заходим в него и идём налево до упора
			if (node->right != nil) {
				prev = node;
				node = node->right;

				while (node->left != nil) {
					prev = node;
					node = node->left;
				}
			}
			else {
				// Если текущая позиция - слева от вершины выше, поднимаемся в неё
				if (prev != nil && prev->right != node) {
					node = prev;
					prev = prev->parent;
				}
				// Если текущая позиция - справа от вершины выше, 
				// поднимаемся до вершины, начиная с которой путь от корня до текущей
				// вершины идёт строго направо, и потом ещё на уровень выше
				else {
					while (prev != nil && prev->right == node) {
						node = prev;
						prev = prev->parent;
					}

					if (prev == nil)
						node = nil;
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
			// Действуем с точностью до наоборот
			if (node->left != nil) {
				prev = node;
				node = node->left;

				while (node->right != nil) {
					prev = node;
					node = node->right;
				}
			}
			else {
				if (prev != nil && prev->left != node) {
					node = prev;
					prev = prev->parent;
				}
				else {
					while (prev != nil && prev->left == node) {
						node = prev;
						prev = prev->parent;
					}

					if (prev == nil)
						node = nil;
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

			return *this;
		}

		bool operator==(const iterator &it) const {
			return node == it.node;
		}
		bool operator!=(const iterator &it) const {
			return node != it.node;
		}
	};

	iterator begin() const {
		Node *curr = root;

		while (curr->left != nil)
			curr = curr->left;

		return iterator(curr, curr->parent, nil);
	}

	iterator end() const {
		return iterator(nil, nil, nil);
	}

	// COMMON PART

	RBTree(const Pred &cmp = Pred()): cmp(cmp) {
		nil->parent = nil->left = nil->right = nil;
		root = nil;
	}

	template <typename Iterator>
	RBTree(const Iterator &first, const Iterator &last, const Pred &cmp = Pred()): cmp(cmp) {
		nil->parent = nil->left = nil->right = nil;
		root = nil;

		for (auto it = first; it != last; ++it) {
			insert(*it);
			++_size;
		}
	}

	RBTree(const RBTree &copy): _size(copy._size), cmp(copy.cmp) {
		nil->parent = nil->left = nil->right = nil;

		_copy_tree(nil, copy.nil);
		root = nil->right;
	}

	RBTree(RBTree &&mov): _size(mov._size), cmp(std::move(mov.cmp)) {
		nil->parent = nil;
		root = nil->right = mov.nil->right;
		mov.nil->right = mov.nil;
	}

	~RBTree() {
		clear();
	}

	RBTree& operator=(const RBTree &copy) {
		if (this != &copy) {
			_size = copy._size;
			cmp = copy.cmp;

			if (root != nullptr)
				_delete_tree(root);
			_copy_tree(nil, copy.nil);
			root = nil->right;
		}

		return *this;
	}

	RBTree& operator=(RBTree &&mov) {
		_size = mov._size;
		cmp = std::move(mov.cmp);

		nil->right = mov.nil->right;
		if (nil != mov.nil)
			mov.nil->right = nullptr;
		root = nil->right;

		return *this;
	}

	size_t size() { return _size; }
	bool empty() { return (_size == 0); }
	void clear() {
		if (root != nullptr)
			_delete_tree(root);
		_size = 0;
	}


	iterator find(const Value &val) const {
		Node *curr = root;
		Node *prev = nil;

		while (curr != nil) {
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

		return iterator(curr, prev, nil);
	}

	iterator insert(const Value &val) {
		// Проверка наличия вершины в дереве
		iterator it = find(val);
		if (it.node != nil)
			throw "Element is already inserted";

		// Вставка новой вершины
		++_size;
		Node *prev = it.prev->parent;
		Node *curr = it.prev;
		Node *next;

		if (curr == nil) // Дерево пустое
			next = nil->right = root = new Node(val, Black, nil, nil, nil);
		else {
			if (cmp(val, curr->data)) // Вставка слева
				next = curr->left = new Node(val, Red, nil, nil, curr);
			else // Вставка справа
				next = curr->right = new Node(val, Red, nil, nil, curr);
		}
		it.node = next;
		it.prev = curr;

		// Балансировка
		while (curr->colour == Red) {
			if (curr == nil || curr == root) // При достижении корня или головы перекрашиваем и выходим
				break;

			if (prev->left == curr) { // Проблемные вершины находятся в левом поддереве предка
				if (curr->right == next) { // Вращение на случай, если вторая вершина лежит в правом поддереве текущей вершины
					_rotate_left(curr);
					curr = next;
				}

				if (prev->right->colour == Red) { // Дядя красный => recolour
					curr->colour = prev->right->colour = Black;
					prev->colour = Red;
				}
				else { // Дядя чёрный => recolour + right rotate (Цвет предка не поменялся, поэтому балансировка завершена)
					curr->colour = Black;
					prev->colour = Red;
					_rotate_right(prev);
					break;
				}
			}
			else { // Проблемные вершины находятся в правом поддереве предка
				if (curr->left == next) {
					_rotate_right(curr);
					curr = next;
				}

				if (prev->left->colour == Red) {
					curr->colour = prev->left->colour = Black;
					prev->colour = Red;
				}
				else {
					curr->colour = Black;
					prev->colour = Red;
					_rotate_left(prev);
					break;
				}
			}

			// Подъём происходит на 2 уровня, потому что на текущей итерации
			// восстановлен баланс для 2 уровней
			next = prev;
			curr = prev->parent;
			prev = prev->parent->parent;
		}
		root->colour = Black;

		return it;
	}

	void erase(iterator it) {
		--_size;
		Node *prev = it.prev; // предок вершины 'curr'
		Node *curr = it.node; // корень дерева, в котором изменилась высота
		Node *erase_node;     // удаляемая вершина (удаление происходит после балансировки, чтобы не сломать дерево)

		// Сводим удаление вершины с 2 непустыми поддеревьями
		// к удалению листа или вершины с 1 непустым поддеревом
		if (curr->left != nil && curr->right != nil) {
			++it;
			curr->data = std::move(it.node->data);

			curr = it.node;
			prev = it.prev;
		}

		erase_node = curr;
		// Удаляем лист
		if (curr->left == nil && curr->right == nil) {
			

			// Балансировка
			if (curr->colour == Black && curr != root) { // Чёрный лист? Много случаев...
				// Подъём с перекраской, если предок текущей вершины, брат и его потомки - чёрные
				// (Это единственный случай, когда невозможно сохранить чёрную высоту в поддереве)
				while (curr != root) {
					if (prev->colour == Black) {
						if (prev->right == curr && prev->left->colour == Black &&
							prev->left->left->colour == Black && prev->left->right->colour == Black) {
							prev->left->colour = Red;
							curr = prev;
							prev = prev->parent;
							continue;
						}
						else if (prev->left == curr && prev->right->colour == Black &&
								 prev->right->left->colour == Black && prev->right->right->colour == Black) {
							prev->right->colour = Red;
							curr = prev;
							prev = prev->parent;
							continue;
						}
					}
					break;
				}

				if (curr != root) {
					Node *brother;

					// Текущее поддерево - правое для предка
					if (prev->right == curr) {
						brother = prev->left;

						// Предок красный
						if (prev->colour == Red) {

							// Есть красный племянник
							if (brother->left->colour == Red || brother->right->colour == Red) {
								if (brother->left->colour == Black)
									_rotate_left(brother);

								prev->colour = prev->left->left->colour = Black;
								prev->left->colour = Red;
								_rotate_right(prev);
							}
							// Оба племянника чёрные
							else {
								prev->colour = Black;
								brother->colour = Red;
							}
						}
						// Предок чёрный
						else {
							// Брат красный
							if (brother->colour == Red) {
								// У правого потомка брата есть красный потомок
								if (brother->right->left->colour == Red || brother->right->right->colour == Red) {
									// Если красный потомок только справа, нужно сделать вращение
									if (brother->right->left->colour == Black)
										_rotate_left(brother->right);

									brother->right->colour = Black;
									brother->right->left->colour = Black;
									_rotate_left(brother);
									_rotate_right(prev);
								}
								// Оба потомка чёрные
								else {
									brother->colour = Black;
									brother->right->colour = Red;
									_rotate_right(prev);
								}
							}
							// Брат чёрный
							else {
								// У брата есть красный потомок
								if (brother->left->colour == Red || brother->right->colour == Red) {
									// Если красный потомок только в правом поддереве, нужно сделать вращение
									if (brother->left->colour == Black)
										_rotate_left(brother);

									prev->left->colour = prev->left->left->colour = Black;
									_rotate_right(prev);
								}
								// В противном случае происходят перекраска и подъём,
								// происходящие в цикле в начале балансировки
							}
						}
					}
					// Симметричный случай для левого поддерева
					else {
						brother = prev->right;

						if (prev->colour == Red) {

							if (brother->right->colour == Red || brother->left->colour == Red) {
								if (brother->right->colour == Black)
									_rotate_right(brother);

								prev->colour = prev->right->right->colour = Black;
								prev->right->colour = Red;
								_rotate_left(prev);
							}
							else {
								prev->colour = Black;
								brother->colour = Red;
							}
						}
						else {
							if (brother->colour == Red) {
								if (brother->left->right->colour == Red || brother->left->left->colour == Red) {
									if (brother->left->right->colour == Black)
										_rotate_right(brother->left);

									brother->left->colour = Black;
									brother->left->right->colour = Black;
									_rotate_right(brother);
									_rotate_left(prev);
								}
								else {
									brother->colour = Black;
									brother->left->colour = Red;
									_rotate_left(prev);
								}
							}
							else {
								if (brother->right->colour == Red || brother->left->colour == Red) {
									if (brother->right->colour == Black)
										_rotate_right(brother);

									prev->right->colour = prev->right->right->colour = Black;
									_rotate_left(prev);
								}
							}
						}
					}
				}
			}
		}

		// Удаляем вершину с 1 непустым поддеревом:
		// 1) слева
		else if (curr->left != nil) {
			curr->left->colour = Black;
			if (curr->parent->left == curr)
				curr->parent->left = curr->left;
			else
				curr->parent->right = curr->left;
			curr->left->parent = curr->parent;
		}

		// 2) справа
		else {
			curr->right->colour = Black;
			if (curr->parent->left == curr)
				curr->parent->left = curr->right;
			else
				curr->parent->right = curr->right;
			curr->right->parent = curr->parent;
		}

		// Удаление вершины
		if (erase_node->parent->left == erase_node)
			erase_node->parent->left = nil;
		else if (erase_node->parent->right == erase_node)
			erase_node->parent->right = nil;
		delete erase_node;

		root = nil->right;
	}

	void erase(const Value &val) {
		// Проверка наличия вершины в дереве
		auto it = find(val);
		if (it.node == nil)
			throw "Element isn't in here";
		
		erase(it);
	}

private:

	void _rotate_left(Node *curr) {
		Node *tmp = curr->right;

		if (tmp->left != nil)
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
	}

	void _rotate_right(Node *curr) {
		Node *tmp = curr->left;

		if (tmp->right != nil)
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
	}

	// Копирование дерева
	void _copy_tree(Node *old_tree, Node *new_tree) {
		if (old_tree->left == nil && old_tree->right == nil)
			return;
		if (old_tree->left != nil) {
			new_tree->left = new Node(old_tree->left->data, old_tree->left->colour, nil, nil, new_tree);
			_copy_tree(old_tree->left, new_tree->left);
		}
		if (old_tree->right != nil) {
			new_tree->right = new Node(old_tree->right->data, old_tree->right->colour, nil, nil, new_tree);
			_copy_tree(old_tree->right, new_tree->right);
		}
	}

	// Удаление дерева
	void _delete_tree(Node *tree) {
		if (tree->left != nil)
			_delete_tree(tree->left);
		if (tree->right != nil)
			_delete_tree(tree->right);
		delete tree;
	}
};
