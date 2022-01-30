#pragma once
#include <iostream>
#include <algorithm>
#include "set"
#include "vector"
#include "map"
#include <unordered_map>
#include "string"
#include <sstream>
#include <gvc.h>
#include <cgraph.h>

namespace MyLib {

	enum class TypeOfNode
	{
		unknown,
		a_node,
		plus_node,
		and_node,
		or_node,
		capture_node,
	};

	class Node { // узел дерева
	private:
		TypeOfNode type; // тип нода
		std::pair<int, std::string> val; // (нумерованная позиция символа алфавита, значение) если -1, то метасимвол;

		bool nullable; // зануляемый ли узел
		std::set<int> first; // мн-во первых позиций для узла
		std::set<int> last; // мн-во последних позиций для узла

		Node* left;
		Node* right;
		Node* parent;

	public:
		Node(std::string info = "", TypeOfNode node = TypeOfNode::unknown, int num = -1, bool isnull = false,
			Node* l = nullptr, Node* r = nullptr, Node* p = nullptr)
			: left(l), right(r), parent(p), type(node), nullable(isnull) {
			val = std::make_pair(num, info);
		}

		Node(const Node& node) : type(node.type), nullable(node.nullable),
			first(node.first), last(node.last), left(nullptr), right(nullptr), parent(nullptr) {
			val = std::make_pair(-1, node.val.second);
		}

		void SetType(const TypeOfNode t) {
			this->type = t;
		}

		TypeOfNode GetType() const {
			return this->type;
		}

		std::pair<int, std::string> GetVal() const {
			return this->val;
		}

		void SetVal(const std::pair<int, std::string> v) {
			this->val = v;
		}

		int GetNum() const {
			return this->val.first;
		}

		void SetNum(const int id) {
			this->val.first = id;
		}

		void SetLeft(Node* node) {
			this->left = node;
		}

		void SetRight(Node* node) {
			this->right = node;
		}

		void SetParent(Node* node) {
			this->parent = node;
		}

		Node* GetLeft() const {
			return this->left;
		}

		Node* GetRight() const {
			return this->right;
		}

		Node* GetParent() const {
			return this->parent;
		}

		bool GetNullable() const {
			return this->nullable;
		}

		void SetNullable(const bool val) {
			this->nullable = val;
		}

		std::set<int>& GetFirst() {
			return this->first;
		}

		void SetFirst(const std::set<int>& val) {
			this->first = val;
		}

		std::set<int>& GetLast() {
			return this->last;
		}

		void SetLast(const std::set<int>& val) {
			this->last = val;
		}

		std::string TypeToStr() const;

		friend std::ostream& operator<< (std::ostream& out, const Node& n);
	};
}