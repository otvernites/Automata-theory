#pragma once

#include "Lexer.h"

namespace MyLib {

	class DFA: public Lexer {

	private:

		// нумерация символов алфавита
		void Numbering(Node* root);

		// Проверить обнуляемость 
		void FindNullable(Node* root);

	public:

		DFA(Node* r = nullptr): Lexer(r) {

		}

		// обертка для алгоритма преобразования
		void Re2DFA(Node* root);

		// изображение DFA
		void CreateDFAImg();
	};
}