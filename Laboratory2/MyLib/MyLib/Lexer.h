#pragma once
#include "Node.h"

namespace MyLib {

	inline std::set<char> metacharacters = { '#', '^', '|', '.', '+', '{', '}', '(', ')', '\\' };

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

		// �������� ���������
		void DeleteSubtree(Node* root);

		// ���������� ������ ������� (��������� �� ������ ����� ����), ��������� �� ����� g_it
		Node* EmbedGroup(int value, const std::vector<Node*>::iterator& g_it);

		// ��������� � ����� ��������� node ������������
		Node* EmbedGroup(int value, Node* node, Node* origin);

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


