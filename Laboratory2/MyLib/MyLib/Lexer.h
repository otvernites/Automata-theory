#pragma once
#include "Node.h"

namespace MyLib {

	inline std::set<char> metacharacters = { '#', '^', '|', '.', '+', '{', '}', '(', ')', '\\' };

	class Lexer { // синтаксическое дерево
	private:
		Node* root;

		// проверка, €вл€етс€ ли строка числом
		bool IsNumber(const std::string s);  

		// подаем строку и позицию первой скобки (если второй аргумент совпадает с первым, то это одиночный символ)
		std::pair<int, int> FindCLosestPair(const std::string& str, const int position); 						 			
		
		// список всех групп захвата + добавочна€ ф-ци€ к ParseRe, здесь объедин€ем группы захвата, если они были разными токенами
		void CaptGroupDetecting(std::string str, std::vector<std::string>& numbers, std::vector<std::string>& tokens);

		// копируем поддерево
		Node* CopyBranch(Node* node);

		// восстанавливаем группы захвата
		void GroupPaste(std::vector<Node*>& capt_groups);

		// удала€ем поддерево
		void DeleteSubtree(Node* root);

		// встраиваем группу повтора (двигаемс€ по правой ветке вниз), вставл€ем на место g_it
		Node* EmbedGroup(int value, const std::vector<Node*>::iterator& g_it);

		// вставл€ем в левое поддерево node наспамленные
		Node* EmbedGroup(int value, Node* node, Node* origin);

		// восстанавливаем повтор€ющиес€ выражени€
		void RepeatPaste(std::vector<Node*>& repeat_nodes);

		// сам алгоритм построени€
		void TreeAlgorithm(int br_count, std::vector<Node*>& syn_tree);

		// дл€ функции печати
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

		// парсим токены в вектор 
		void ParseRe(std::string& str, std::vector<std::string>& vec); 

		// обертка дл€ алгоритма построени€ дерева
		void Re2Tree(std::string& str);  

		// изображение дерева
		void CreateTreeImg();
	};
}


