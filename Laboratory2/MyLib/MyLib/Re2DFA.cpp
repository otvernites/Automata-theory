#include "Re2DFA.h"

namespace MyLib {

	// ----
	void DFA::Numbering(Node* root, std::unordered_map<int, std::string>& map) {

		static int flag = -1;

		if (root == nullptr)
			return;

		if (flag < INT_MAX) {

			// левая рекурсия
			Numbering(root->GetLeft(), map);

			// правая рекурсия
			Numbering(root->GetRight(), map);

			++flag;

			// устанавливаем значение для a-node
			if (root->GetType() == TypeOfNode::a_node) {
				root->SetNum(flag);
				std::string str = root->GetVal().second;
				//if (str != "^") {
				if ((str != "$") && (str != "^")) {
					alphabet.insert(root->GetVal().second); // одновременно создаем алфавит (по которому будут строиться переходы)
				}
				map[flag] = root->GetVal().second; // карта соотвествий символ - позиция
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
		case TypeOfNode::capture_node:
			node->SetNullable(node->GetRight()->GetNullable());
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
		case TypeOfNode::capture_node:
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
			node->SetLast(node->GetLeft()->GetLast());
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
		case TypeOfNode::capture_node:
			node->GetLast().insert(node->GetRight()->GetLast().begin(), node->GetRight()->GetLast().end());
			break;
		}
	}

	void DFA::FindFollowPos(Node* node, std::vector<std::set<int>>& follow_pos) {
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

	void DFA::TreeTraversal(Node* root, std::vector<std::set<int>>& follow_pos) {
		if (root) {
			TreeTraversal(root->GetLeft(), follow_pos);
			TreeTraversal(root->GetRight(), follow_pos);

			FindNullable(root);
			FindFirst(root);
			FindLast(root);
			FindFollowPos(root, follow_pos);
		}
	}
	// ----

	void DFA::CreatingStates(std::unordered_map<int, std::string>& pos_map, std::vector<std::set<int>>& follow_pos) { // pos_map - не для всех a_node
		// создаем нулевое состояние																					// follow_pos для всех
		State* empty_state = new State();

		// создаем стартовое состояние
		start = new State();
		start->positions = GetRoot()->GetFirst();
		states.push_back(start);
		int i = -1; // счетчик для итерации по вектору состояний

		// обработка следующих состояний
		do {  // обрабатываем одно состояние
			for (auto& symb : alphabet) {

				std::set<int> new_state_pos; // объединение followpos
				for (auto& pos : (*(states[i + 1])).positions) {
					if (pos_map[pos] == symb){ // чтобы после конца-доллара не ссылаться на что-то
						if (!follow_pos[pos].empty()) { // если где-то встречается эпсилон, то в follow_pos будет пустое множ-во, а в pos_map не будет (по индексу такому)
							new_state_pos.insert(follow_pos[pos].begin(), follow_pos[pos].end());
						} 
						else { // поэтому при разных индексах, мы их уравниваем
							follow_pos.erase(follow_pos.begin() + pos);
							new_state_pos.insert(follow_pos[pos].begin(), follow_pos[pos].end());
						}					
					}
				}

				// сравниваем с остальными состояниями, новое оно или уже существует
				int flag = 1; // новое
				State* mark_state = nullptr; // чтобы сохранить совпавшее состояние
				for (auto state : states) {
					if (state->positions == new_state_pos) {
						flag = 0; // уже существует
						mark_state = state;
						break;
					}
				}

				if ((flag == 1) && (!new_state_pos.empty())) { // если состояние новое и не ушло в ноль
					State* new_state = new State();
					new_state->positions = new_state_pos;
					states.push_back(new_state);

					(states[i + 1])->transitions.insert(std::pair<std::string, State*>(symb, states[states.size() - 1]));  // создаем переход
				}
				else if (flag == 1) { // если оно ушло в ноль
					auto empty_pos = std::find(states.begin(), states.end(), empty_state);
					if (empty_pos == states.end()) { // если нуля еще не было
						states.push_back(empty_state);
						(states[i + 1])->transitions.insert(std::pair<std::string, State*>(symb, states[states.size() - 1]));
					}
				}
				else { // если состояние уже существовало
					(states[i + 1])->transitions.insert(std::pair<std::string, State*>(symb, mark_state));
				}

				new_state_pos.clear();
			}
			++i; // пометила, что состояние обработано
		} while (states.back() != states[i]); // если текущий эл-т и конец не совпадают, есть еще состояния для обработки

		// добавляем принимающие состояния в вектор
		int end_pos = 0; // определяем номер позиции со строкой $
		for (auto it = pos_map.begin(); it != pos_map.end(); ++it) {
			if ((*it).second == "$") {
				end_pos = (*it).first;
			}
		}
			
		for (auto state : states) { // если $ входит в состояние, то оно принимающее
			if ((*state).positions.contains(end_pos)) {
				accepting_states.insert(state);
			}
		}
		pos_map.clear();
		follow_pos.clear();
	} 

	void DFA::DFAMinimization() {

	}

	void DFA::PrintState(std::ostringstream& buffer) {

		int id_from = 0;
		for (auto state : states) {
			buffer << id_from << " [";
			if (state == start) {
				buffer << "shape=\"" << "circle" << "\"";
			}
			else if (accepting_states.contains(state)) {
				buffer << "shape=\"" << "doublecircle" << "\"";
			}
			else {
				buffer << "shape=\"" << "ellipse" << "\"";
			}

			buffer << "label=\"";
			for (auto pos : state->positions) {
				buffer << pos << " ";
			}
			buffer << "\"];\n";

			for (auto tran : state->transitions) {
				auto it = std::find(states.begin(), states.end(), tran.second);
				int id_to = std::distance(states.begin(), it);
				buffer << id_from << "->" << id_to << " [label=\" " << tran.first << " \"];\n";
			}
			++id_from;
		}
	}

	void DFA::CreateDFAImg() {

		std::ostringstream buffer("", std::ios_base::ate);

		buffer << "digraph G {\n";
		PrintState(buffer);
		buffer << "}";

		Agraph_t* g = agmemread(buffer.str().c_str());
		GVC_t* gvc = gvContext();
		gvLayout(gvc, g, "dot");
		gvRenderFilename(gvc, g, "png", ".\\dfa.png");
		gvFreeLayout(gvc, g);
		agclose(g);
		gvFreeContext(gvc);
	}

	void DFA::Re2DFA() {

		Node* root = GetRoot(); 

		// 1 шаг - нумерация символов алфавита
		std::unordered_map<int, std::string> conformity; // соответствие позиция-символ алфавита (нужна для построения состояний)
		Numbering(root, conformity);

		// 2 шаг - N, F, L, FP
		std::vector<std::set<int>> follow_pos; // множество всех следующих позиций
		Node* ptr = GetRoot();
		while (ptr->GetRight() != nullptr) {
			ptr = ptr->GetRight();
		}
		follow_pos.resize(ptr->GetNum());
		TreeTraversal(root, follow_pos);

		// 3 шаг - создание состояний
		CreatingStates(conformity, follow_pos);

		// 4 шаг - минимизация ДКА
		DFAMinimization();
	}
}
