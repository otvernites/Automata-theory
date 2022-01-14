#pragma once

#include "Lexer.h"

namespace MyLib {

	class DFA: public Lexer {

	private:
		std::vector<std::set<int>> follow_pos; // множество следующих позиций

		// Нумерация символов алфавита
		void Numbering(Node* node);

		// Проверить обнуляемость 
		void FindNullable(Node* node);

		// Построить множества первых позиций
		void FindFirst(Node* node);

		// Построить множество последних позиций
		void FindLast(Node* node);

		// Построить множество следующих позиций
		void FindFollowPos(Node* node);

		// Для одновременного построения множеств 
		void TreeTraversal(Node* root);

	public:

		DFA(Node* r = nullptr): Lexer(r) {

		}

		// обертка для алгоритма преобразования
		void Re2DFA(Node* root);

		// изображение DFA
		void CreateDFAImg();
	};
}