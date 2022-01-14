#include "Re2DFA.h"

namespace MyLib {

	void DFA::Numbering(Node* root) {

		static int flag = -1;

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

	void DFA::FindNullable(Node* node) {
		switch (node->GetType()) {
		case TypeOfNode::a_node:
			if (node->GetVal().second == "^") {
				node->SetNullable(true);
			}
			else {
				node->SetNullable(false);
			}
			break;
		case TypeOfNode::plus_node:
			node->SetNullable(node->GetLeft()->GetNullable());
			break;
		case TypeOfNode::and_node:
			node->SetNullable(node->GetLeft()->GetNullable() && node->GetRight()->GetNullable());
			break;
		case TypeOfNode::or_node:
			node->SetNullable(node->GetLeft()->GetNullable() || node->GetRight()->GetNullable());
			break;
		}
	}
	
	void DFA::FindFirst(Node* node) {
		switch (node->GetType()) {
		case TypeOfNode::a_node:
			if (node->GetVal().second != "^") {
				node->GetFirst().insert(node->GetVal().first);
			}
			break;
		case TypeOfNode::plus_node:
			node->SetFirst(node->GetLeft()->GetFirst());
			break;
		case TypeOfNode::and_node:
			if (node->GetLeft()->GetNullable()) {
				node->GetFirst().insert(node->GetLeft()->GetFirst().begin(), node->GetLeft()->GetFirst().end());
				node->GetFirst().insert(node->GetRight()->GetFirst().begin(), node->GetRight()->GetFirst().end());
			}
			else {
				node->SetFirst(node->GetLeft()->GetFirst());
			}
			break;
		case TypeOfNode::or_node:
			node->GetFirst().insert(node->GetLeft()->GetFirst().begin(), node->GetLeft()->GetFirst().end());
			node->GetFirst().insert(node->GetRight()->GetFirst().begin(), node->GetRight()->GetFirst().end());
			break;
		}
	}

	void DFA::FindLast(Node* node) {
		switch (node->GetType()) {
		case TypeOfNode::a_node:
			if (node->GetVal().second != "^") {
				node->GetLast().insert(node->GetVal().first);
			}
			break;
		case TypeOfNode::plus_node:
			node->SetLast(node->GetRight()->GetLast());
			break;
		case TypeOfNode::and_node:
			if (node->GetRight()->GetNullable()) {
				node->GetLast().insert(node->GetLeft()->GetLast().begin(), node->GetLeft()->GetLast().end());
				node->GetLast().insert(node->GetRight()->GetLast().begin(), node->GetRight()->GetLast().end());
			}
			else {
				node->SetLast(node->GetRight()->GetLast());
			}
			break;
		case TypeOfNode::or_node:
			node->GetLast().insert(node->GetLeft()->GetLast().begin(), node->GetLeft()->GetLast().end());
			node->GetLast().insert(node->GetRight()->GetLast().begin(), node->GetRight()->GetLast().end());
			break;
		}
	}

	void DFA::FindFollowPos(Node* node) {
		follow_pos.resize(GetRoot()->GetRight()->GetNum());
		switch (node->GetType()) {
		case TypeOfNode::plus_node:
			for (int pos : node->GetLeft()->GetLast()) {
				follow_pos[pos].insert(node->GetLeft()->GetFirst().begin(), node->GetLeft()->GetFirst().end());
			}
			break;
		case TypeOfNode::and_node:
			for (int pos : node->GetLeft()->GetLast()) {
				follow_pos[pos].insert(node->GetRight()->GetFirst().begin(), node->GetRight()->GetFirst().end());
			}
			break;
		}
	}

	void DFA::TreeTraversal(Node* root) {
		if (root) {
			TreeTraversal(root->GetLeft());
			TreeTraversal(root->GetRight());

			FindNullable(root);
			FindFirst(root);
			FindLast(root);
			FindFollowPos(root);
		}
	}

	void DFA::CreateDFAImg() {}

	void DFA::Re2DFA(Node* root) {

		// 1 шаг - нумерация символов алфавита
		Numbering(root);

		// 2 шаг - N, F, L, FP
		TreeTraversal(root);

	}
}
