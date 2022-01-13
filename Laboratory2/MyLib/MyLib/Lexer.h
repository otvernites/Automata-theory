#pragma once

#include <iostream>
#include "set"
#include "vector"
#include "string"
#include <sstream>
#include <gvc.h>
#include <cgraph.h>

namespace MyLib {

	inline std::set<char> metacharacters = { '#', '^', '|', '.', '+', '{', '}', '(', ')', '\\' };

	enum class TypeOfNode
	{
		unknown,
		a_node,
		plus_node,
		and_node,
		or_node,
		capture_node,
	};

	class Node { // ���� ������
	private:
		TypeOfNode type; // ��� ����
		std::pair<int, std::string> val; // (������������ ������� ������� ��������, ��������) ���� -1, �� ����������;
		
		bool nullable; // ���������� �� ����
		std::set<int> first; // ��-�� ������ ������� ��� ����
		std::set<int> last; // ��-�� ��������� ������� ��� ����

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

		std::string TypeToStr() const {
			switch (this->GetType()) {
			case TypeOfNode::a_node:	   return "a_node";
			case TypeOfNode::plus_node:    return "plus_node";
			case TypeOfNode::and_node:     return "and_node";
			case TypeOfNode::or_node:      return "or_node";
			case TypeOfNode::capture_node: return "capture_node";
			default:					   return "";
			};
		}

		friend std::ostream& operator<< (std::ostream& out, const Node& n) {

			out << n.TypeToStr() << " ( ";

			switch (n.nullable) {
			case false:
				out << "-";
				break;
			case true:
				out << "+";
				break;
			};

			out << " {" << n.GetVal().first << "}  " << n.GetVal().second << " )";
			return out;
		}
	};

	class Lexer { // �������������� ������
	private:
		Node* root;

		// ��������, �������� �� ������ ������
		bool IsNumber(const std::string s);  

		// ������ ������ � ������� ������ ������ (���� ������ �������� ��������� � ������, �� ��� ��������� ������)
		std::pair<int, int> FindCLosestPair(const std::string& str, const int position); 						 			
		
		// ������ ���� ����� ������� + ���������� �-��� � ParseRe, ����� ���������� ������ �������, ���� ��� ���� ������� ��������
		void CaptGroupDetecting(std::string str, std::vector<std::string>& numbers, std::vector<std::string>& tokens);

		// �������� ���������
		Node* CopyBranch(Node* node);

		// ��������������� ������ �������
		void GroupPaste(std::vector<Node*>& capt_groups);

		// ��������������� ������������� ���������
		void RepeatPaste(std::vector<Node*>& repeat_nodes);

		// ��� �������� ����������
		void TreeAlgorithm(int br_count, std::vector<Node*>& syn_tree);

		// ��� ������� ������
		void PrintNode(const Node* root, std::ostringstream& buffer, int id);

	public:
		Lexer(Node* r = nullptr) : root(r) {}

		Lexer& operator = (const Lexer& tree);

		Node* GetRoot() const {
			return this->root;
		}

		void SetRoot(Node* ptr) {
			this->root = ptr;
		}

		// ������ ������ � ������ 
		void ParseRe(std::string& str, std::vector<std::string>& vec); 

		// ������� ��� ��������� ���������� ������
		void Re2Tree(std::string& str);  

		// ����������� ������
		void CreateTreeImg();
	};
}


