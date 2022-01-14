#pragma once

#include "Lexer.h"

namespace MyLib {

	class DFA: public Lexer {

	private:
		std::vector<std::set<int>> follow_pos; // ��������� ��������� �������

		// ��������� �������� ��������
		void Numbering(Node* node);

		// ��������� ������������ 
		void FindNullable(Node* node);

		// ��������� ��������� ������ �������
		void FindFirst(Node* node);

		// ��������� ��������� ��������� �������
		void FindLast(Node* node);

		// ��������� ��������� ��������� �������
		void FindFollowPos(Node* node);

		// ��� �������������� ���������� �������� 
		void TreeTraversal(Node* root);

	public:

		DFA(Node* r = nullptr): Lexer(r) {

		}

		// ������� ��� ��������� ��������������
		void Re2DFA(Node* root);

		// ����������� DFA
		void CreateDFAImg();
	};
}