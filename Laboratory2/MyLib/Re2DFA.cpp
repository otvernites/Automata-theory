#include "Re2DFA.h"

namespace MyLib {

	void DFA::Numbering(Node* root) {

		static int flag = 0;

		if (root == nullptr)
			return;

		if (flag < INT_MAX) {

			// левая рекурсия
			Numbering(root->GetLeft());

			// правая рекурсия
			Numbering(root->GetRight());

			++flag;

			// устанавливаем значение для a-node
			if (root->GetType() == TypeOfNode::a_node) {
				root->SetNum(flag);
			}
			else {
				--flag;
			}	
		}
	}

	void DFA::FindNullable(Node* root) {


	}

	void DFA::CreateDFAImg() {}

	void DFA::Re2DFA(Node* root) {

		// 1 шаг - нумерация символов алфавита
		this->Numbering(root);

		// 2 шаг - помечаем, зануляемый ли это символ
		this->FindNullable(root);
	}
}
