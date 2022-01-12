#include "Re2DFA.h"

namespace MyLib {

	void DFA::Numbering(Node* root) {

		static int flag = 0;

		if (root == nullptr)
			return;

		if (flag < INT_MAX) {

			// ����� ��������
			Numbering(root->GetLeft());

			// ������ ��������
			Numbering(root->GetRight());

			++flag;

			// ������������� �������� ��� a-node
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

		// 1 ��� - ��������� �������� ��������
		this->Numbering(root);

		// 2 ��� - ��������, ���������� �� ��� ������
		this->FindNullable(root);
	}
}
