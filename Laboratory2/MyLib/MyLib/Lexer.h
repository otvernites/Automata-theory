#pragma once
#include "Node.h"

namespace MyLib {

	inline std::set<char> metacharacters = { '#', '|', '.', '+', '{', '}', '(', ')', '\\'};

	class Lexer { // синтаксическое дерево
	private:
		Node* root;

		// проверка, является ли строка числом
		bool IsNumber(const std::string s);  

		// подаем строку и позицию первой скобки (если второй аргумент совпадает с первым, то это одиночный символ)
		std::pair<int, int> FindCLosestPair(const std::string& str, const int position); 						 			
		
		// список всех групп захвата + добавочная ф-ция к ParseRe, здесь объединяем группы захвата, если они были разными токенами
		void CaptGroupDetecting(std::string str, std::vector<std::string>& numbers, std::vector<std::string>& tokens);

		// копируем поддерево
		Node* CopyBranch(Node* node);

		// восстанавливаем группы захвата
		void GroupPaste(std::vector<Node*>& capt_groups);

		// удалаяем поддерево
		void DeleteSubtree(Node* root);

		// встраиваем группу повтора (двигаемся по правой ветке вниз), вставляем на место g_it
		Node* EmbedGroup(int value, const std::vector<Node*>::iterator& g_it);

		// вставляем в левое поддерево node наспамленные
		Node* EmbedGroup(int value, Node* node, Node* origin);

		// заполняем вектор местонахождениями plus_node
		void InorderForReapeatGr(std::vector<Node*>& repeat_nodes, Node* current_node);

		// восстанавливаем повторяющиеся выражения
		void RepeatPaste(std::vector<Node*>& repeat_nodes);

		// сам алгоритм построения
		void TreeAlgorithm(int br_count, std::vector<Node*>& syn_tree);

		// для функции печати
		void PrintNode(const Node* root, std::ostringstream& buffer, int id);


	public:
		Lexer(Node* r = nullptr) : root(r) {}

		~Lexer() {
			if (root != nullptr) {
				DeleteSubtree(root);
			}
		}

		Node* GetRoot() const {
			return this->root;
		}

		void SetRoot(Node* ptr) {
			this->root = ptr;
		}

		Lexer& operator = (const Lexer& tree);

		// парсим токены в вектор 
		void ParseRe(std::string& str, std::vector<std::string>& vec); 

		// обертка для алгоритма построения дерева
		void Re2Tree(std::string& str);  

		// изображение дерева
		void CreateTreeImg();
	};
}


