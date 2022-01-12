#pragma once

#include "Lexer.h"

namespace MyLib {

	class DFA: public Lexer {

	private:

		// ��������� �������� ��������
		void Numbering(Node* root);

		// ��������� ������������ 
		void FindNullable(Node* root);

	public:

		DFA(Node* r = nullptr): Lexer(r) {

		}

		// ������� ��� ��������� ��������������
		void Re2DFA(Node* root);

		// ����������� DFA
		void CreateDFAImg();
	};
}