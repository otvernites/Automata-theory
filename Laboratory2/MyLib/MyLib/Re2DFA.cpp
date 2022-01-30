#include "Re2DFA.h"

namespace MyLib {

	// ----
	void DFA::Numbering(Node* root, std::vector<int> vec_capture_groups) {

		static int flag = -1;
		
		if (root && (root->GetType() == TypeOfNode::capture_node)) {
			if ((!vec_capture_groups.size()) || (vec_capture_groups.back() != atoi(root->GetVal().second.c_str()))) {
				vec_capture_groups.push_back(atoi(root->GetVal().second.c_str()));
			}
			else {
				vec_capture_groups.pop_back();
			}
		}

		if (root == nullptr)
			return;

		if (flag < INT_MAX) {

			// левая рекурсия
			Numbering(root->GetLeft(), vec_capture_groups);

			// правая рекурсия
			Numbering(root->GetRight(), vec_capture_groups);

			++flag;

			// устанавливаем значение для a-node
			if (root->GetType() == TypeOfNode::a_node) {
				root->SetNum(flag);
				std::string str = root->GetVal().second;

				if ((str != "$") && (str != "^")) {
					alphabet.insert(str); // одновременно создаем алфавит (по которому будут строиться переходы)
				}
				conformity[flag] = str; // карта соотвествий символ - позиция

				if (vec_capture_groups.size() > 0) {
					for (auto cap_group : vec_capture_groups) {
						pos_table[cap_group].insert(flag);
					}
				}
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

		//pos_map.clear();
		follow_pos.clear();
	} 

	bool DFA::IsDiffGroups(std::vector<std::set<State*>>& groups, State* first_state, State* second_state, std::string symb) {
		bool flag = false; // одинаковые группы
		for (auto& group : groups) {
			for (auto state : group) {
				if (first_state->transitions[symb] == state) { // если я нашла группу, в которую переходит из первого состояния
					auto it = group.find(second_state->transitions[symb]); // проверяю, в этой же группе переход для второго состояния
					if (it == group.end()) { // разные группы
						flag = true;
						return flag;
					}
				}
			}
		}
		return flag;
	}

	void DFA::DFAMinimization() {

		if (this->states.size() == 1) {
			return;
		}

		std::vector<std::set<State*>> split_groups; // П, вектор множеств состояний, которые мы потом объединим в одно (изменяем)
		
		split_groups.push_back(accepting_states); // добавили туда множество принимающих
		if (states.size() != 1) {
			split_groups.push_back(std::set<State*>());  // и множество оставшихся (S - F)
		}
		for (auto state : states) { 
			if ((!accepting_states.contains(state)) && (states.size() != 1)) {
				split_groups[1].insert(state);
			}
		}
		
		std::vector<std::set<State*>> split_groups_copy; // П, вектор для сравнения с первичным (его не изменяем)
		while (split_groups_copy != split_groups) {

			split_groups_copy = split_groups;

			// основная часть, разбиваю каждую группу на подгруппы
			std::vector<std::set<State*>>::iterator group_it = split_groups.begin();
			int group_num = 0;
			while (group_it != split_groups.end()) {
				if ((*group_it).size() > 1) { // если в группе больше одного состояния

					std::set<State*>::iterator state_it = (*group_it).begin(); // итератор по множ-ву (для первого сост)
					std::set<State*>::iterator other_states_it = state_it; //  итератор по тому же множ-ву (для второго сост)

					std::set<State*> new_group; // создаем группу для разделения
					while (state_it != (*group_it).end()) { // для каждого состояния в группе (первое)

						while (other_states_it != (*group_it).end()) { // проходимся по всем состояниям (второе)
							if (state_it == other_states_it) {
								++other_states_it;
								continue;
							}
							else {
								for (auto& symb : alphabet) {  // проходимся по всем символам алфавита
									if (IsDiffGroups(split_groups, *state_it, *other_states_it, symb)) { // если группы разные, нужно разделять 
										new_group.insert(*other_states_it);
										other_states_it = (*group_it).erase(other_states_it);

										// если оно находится в самом начале, при сдвиге влево будет end, дальше не проверит
										if ((other_states_it != (*group_it).begin()) && (other_states_it != (*group_it).end())) {
											--other_states_it;
										}
										break;
									}
								}
								if (other_states_it != (*group_it).end()) {
									++other_states_it;
								}
							}
						}
						
						if (new_group.size() != 0) {
							// вставляю для одного символа группу найденных отличающихся
							int counter = std::distance((*group_it).begin(), state_it);
							split_groups.insert(split_groups.end(), new_group); // из-за этого итераторы state_it и group_it сбиваются
							group_it = split_groups.begin() + group_num;
							state_it = (*group_it).begin();
							std::advance(state_it, counter);
							other_states_it = state_it;
							new_group.clear();
						}

						++state_it; //перехожу к след состоянию
					}
				}
				++group_num;
				++group_it; // перехожу к след группе
			}
		}
		split_groups_copy.clear();

		// ---------- теперь выбираем представителей для состояний !!! ----------

		std::map<State*, int> from_which_group; // каждому номеру группы в соответствие ставится состояние
		std::vector<State*> main_states;  //каждой группе в соответствие ставим представителя
		int group_num = 0;
		for (auto& states : split_groups) {
			for (auto state : states) {
				from_which_group.insert(std::make_pair(state, group_num));
			}
			if (states.begin() != states.end()) {
				main_states.push_back(*(states.begin()));
				++group_num;
			}
		}
		
		// каждая группа - новое состояние, объединяет в себе несколько состояний
		group_num = 0;
		for (auto& group : split_groups) { // для каждой группы
			if (!group.empty()) {
				std::set<std::string> symbols = alphabet;
				for (auto state : group) { // для каждого состояния в группе

					if (state == start) { // установила новый старт
						start = main_states[group_num];
					}
					else if (accepting_states.contains(state)) { // добавила новые принимающие состояния
						accepting_states.erase(state);
						accepting_states.insert(main_states[group_num]);
					}

					main_states[group_num]->positions.insert(state->positions.begin(), state->positions.end());

					for (auto& tran : state->transitions) { // для каждого перехода в состоянии

						if (from_which_group[tran.second] != group_num) { // если из другой группы
							symbols.erase(tran.first);
							main_states[group_num]->transitions[tran.first] = main_states[from_which_group[tran.second]];
						}
						else if ((symbols.contains(tran.first) && (main_states[group_num]->transitions[tran.first] != main_states[group_num]))) { // из этой же группы и еще не определен
							main_states[group_num]->transitions[tran.first] = main_states[group_num];
						}
					}
				}

				// удаляем все состояния в группе, кроме главного
				if (group.begin() != group.end()) {
					auto it = ++group.begin();
					while (it != group.end()) {
						(*it)->positions.clear();
						(*it)->transitions.clear();
						delete(*it);
						it = group.erase(it);
					}
				}

				++group_num;
			}
		}

		states.clear();
		for (auto& group : split_groups) {
			for (auto state : group) {
				states.push_back(state);
			}
		}

		from_which_group.clear();
		main_states.clear();
		split_groups.clear();

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

			for (auto& tran : state->transitions) {
				auto it = std::find(states.begin(), states.end(), tran.second);
				int id_to = std::distance(states.begin(), it);
				buffer << id_from << "->" << id_to << " [label=\" " << tran.first << " \"];\n";
			}
			++id_from;
		}
	}

	void DFA::CreateDFAImg(const char* f_name) {

		std::ostringstream buffer("", std::ios_base::ate);

		buffer << "digraph G {\n";
		PrintState(buffer);
		buffer << "}";

		Agraph_t* g = agmemread(buffer.str().c_str());
		GVC_t* gvc = gvContext();
		gvLayout(gvc, g, "dot");
		gvRenderFilename(gvc, g, "png", f_name);
		gvFreeLayout(gvc, g);
		agclose(g);
		gvFreeContext(gvc);
	}

	int DFA::GetMinStates() const {
		return states.size();
	}


	// ----

	// ----
	DFA* DFA::Compile(std::string str) {

		// Чистим за собой предыдущее состояние автомата
		for (auto it = states.begin(); it != states.end(); ++it) {
			delete* it;
		}
		states.clear();
		start = nullptr;
		alphabet.clear();
		accepting_states.clear();
		subpaths.clear();

		// 0 шаг - построение дерева
		Re2Tree(str);
		CreateTreeImg();
		Node* root = GetRoot();

		// 1 шаг - нумерация символов алфавита
		
		std::vector<int> stack_capture_groups;
		Numbering(root, stack_capture_groups);

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
		CreateDFAImg(".\\dfa.png");

		// 4 шаг - минимизация ДКА
		DFAMinimization();
		CreateDFAImg(".\\mindfa.png");
		return this;
	}

	void DFA::Analyze_path(std::string correct_str) {
		
		// предобработка строки
		std::set<std::string> metacharacters = { "#", "|", ".", "+", "{", "}", "(", ")", "\\" };
		auto str_it = correct_str.begin();
		while (str_it != correct_str.end()) {
			if (metacharacters.contains(std::string(1, *str_it))) {
				str_it = correct_str.insert(str_it, '#');
				++str_it;
			}
			++str_it;
		}

		for (auto cap_group : pos_table) { // перебираем группы захвата
			State* current_state = GetStart();
			std::string captured_str;
			for (int i = 0; i < correct_str.size(); i++) { //проходимся по строке

				std::string symb;
				if (correct_str.substr(i, 1) == "#") {
					symb = correct_str.substr(i, 2);
					++i;
				}
				else {
					symb = correct_str.substr(i, 1);
				}
				
				std::set<int> pos_set; // сет позиций на которых стоит символ, по которому осуществили переход из данного состояния
				std::set<int> intersection;
				for (auto pos : current_state->positions) {
					if (conformity[pos] == symb) { //проверяем, что есть позиции в состоянии, на которых стоит этот символ. Собираем их в set 
						pos_set.insert(pos);
					}
				}
				// находим позиции, которые входят одновременно и в состояние, из которого переход, и в группу захвата 
				if (pos_set.size() && cap_group.second.size()) {
					std::set_intersection(pos_set.begin(), pos_set.end(), cap_group.second.begin(), cap_group.second.end(), 
						std::inserter(intersection, intersection.begin()));
					if (intersection.size()) {
						captured_str.push_back(correct_str[i]);
					}
					else if (captured_str.size()) {
						group_results[cap_group.first].push_back(captured_str);
						captured_str.clear();
					}
					intersection.clear();
				} 
				current_state = current_state->transitions[symb];
			}
			if (captured_str.size()) {
				group_results[cap_group.first].push_back(captured_str);
				captured_str.clear();
			}
			
		}
	}

	std::vector<std::string> DFA::FindAll(std::string sample) {

		State* current_state = GetStart();
		if (current_state == nullptr) {
			throw std::exception("The automate needs to be initialized!");
		}

		// предобработка строки
		std::set<std::string> metacharacters = { "#", "|", ".", "+", "{", "}", "(", ")", "\\" };
		auto str_it = sample.begin();
		while (str_it != sample.end()) {
			if (metacharacters.contains(std::string(1, *str_it))) {
				str_it = sample.insert(str_it, '#');
				++str_it;
			}
			++str_it;
		}

		std::vector<std::string> results;

		// если стартовое совпадает с принимающим 
		if (accepting_states.contains(current_state)) {
			results.push_back("");
		}

		std::string correct_str, tmp_str;
		size_t i = 0;
		size_t potential_next_id = -1; // флаг, чтобы не упускать строки при проверке

		// начинаем обработку строки
		while (i < sample.size()) {

			std::string symb;
			if (sample.substr(i, 1) == "#") {
				symb = sample.substr(i, 2);
				++i;
			}
			else {
				symb = sample.substr(i, 1);
			}
			
			// работаем только со временным результатом
			if (current_state->transitions.count(symb) > 0) { // если существует переход по этому символу
				current_state = current_state->transitions[symb];
				

				// устанавливаем след. потенциальный индекс
				if ((current_state != GetStart()) && (potential_next_id == -1)) {
					potential_next_id = i + 1;
				}

				if (current_state->positions.size() > 0) { // если след. состояние нормальное
					tmp_str.push_back(sample.at(i));
				}
				else {  // если след. состояние - пустота
					if (correct_str.size() > 0) { // если что-то успело подойти, сохраняем
						results.push_back(correct_str);
						Analyze_path(correct_str);
						potential_next_id = -1;
						correct_str.clear();
						tmp_str.clear();
						current_state = GetStart();
						if (symb.size() > 1) {
							i = i - 2;
						}
						else {
							--i;
						}
					}
					else { // ничего не подошло
						i = potential_next_id;
						potential_next_id = -1;
						current_state = GetStart();
						tmp_str.clear();
						continue;
					}
				}
			}
			else {  // не существует
				if (correct_str.size() > 0) { // если что-то успело подойти, сохраняем
					results.push_back(correct_str);
					Analyze_path(correct_str);
					correct_str.clear();
					potential_next_id = -1;
					tmp_str.clear();
					current_state = GetStart();
					if (symb.size() > 1) {
						i = i - 2;
					}
					else {
						--i;
					}
				}
				else { // ничего не подошло
					current_state = GetStart();
					tmp_str.clear();
				}
			}

			// точно верный результат, т.к. пришло в принимающее состояние
			if (accepting_states.contains(current_state)) {
				correct_str = tmp_str;
				
			}
			++i;
			
			// обработка ситуации, когда индекс дошел до конца, но подстрока еще может подойти
			if ((i == sample.size()) && (correct_str.size() == 0) && (potential_next_id < sample.size())) {
				i = potential_next_id;
				current_state = GetStart();
				tmp_str.clear();
				potential_next_id = -1;
			}
		}

		// добавляем результат в вектор
		if (correct_str.size() > 0) {
			results.push_back(correct_str);
			Analyze_path(correct_str);
			correct_str.clear();
		}
		
		// удаление дубликатов
		sort(results.begin(), results.end());
		results.resize(unique(results.begin(), results.end()) - results.begin());

		return results;
	}
	
	std::vector<std::string> DFA::FindAll(std::string re, std::string sample) {
		Compile(re);
		return FindAll(sample);
	}

	std::string DFA::KPath() {

		if (GetStart() == nullptr) {
			throw std::exception("The automate needs to be initialized!");
		}

		std::vector<std::string> results;

		std::set<std::string> metacharacters = { "#", "|", ".", "+", "{", "}", "(", ")", "\\" };

		subpaths.clear(); // очищаем выражения для промежуточных состояний индукции
		std::string re; // регулярка для ДКА (ее надо составить)

		std::vector<State*> states = GetStates(); // состояния автомата
		std::map<State*, int> numb_states; // состояния автомата (состояние - его номер)
		std::map<std::pair<int, int>, std::vector<std::string>> paths; // для k = 0, здесь вершины-ребра между вершинами

		// Нумеруем состояния 
		int i = 1;
		for (auto st : states) {
			numb_states[st] = i;
			++i;
		}
		int k = numb_states.size(); // номер последнего состояния

		// Строим мапу для k = 0
		for (auto st : states) {
			for (auto& symb : GetAlphabet()) {
							int from = numb_states[st];
							int to = numb_states[st->transitions[symb]];
							paths[std::make_pair(from, to)].push_back(symb);
			}
		}

		// Шаги индукции 
		State* start_state = GetStart();
		for (auto st : GetAcceptStates()) {
			KPathInduction(k, numb_states[start_state], numb_states[st],  paths);
			re += subpaths[std::make_tuple(numb_states[start_state], numb_states[st], k)] + "|";
			if (re == "|") { re.pop_back(); }
		}
		if (!re.empty()) {
			re.pop_back();
		}

		return re;
	}

	void DFA::KPathInduction(int k, int id_from, int id_to, std::map<std::pair<int, int>,std::vector<std::string>> paths) {
		std::string i_j, i_k, k_k, k_j;
		std::set<std::string> metacharacters = { "#", "|", ".", "+", "{", "}", "(", ")", "\\" };

		// строим базис
		if (k == 0) {
			std::string str;
			// базис (k = 0)
			if (paths[std::make_pair(id_from, id_to)].size() > 0) {
				for (auto& path : paths[std::make_pair(id_from, id_to)]) {
					if (metacharacters.find(path) != metacharacters.end()) {
						path = "#" + path;
					}
					str += path + "|";
				}
				str.pop_back();

				if (str.size() > 1) {
					subpaths[std::make_tuple(id_from, id_to, k)] = "(" + str + ")";
				}
				else {
					subpaths[std::make_tuple(id_from, id_to, k)] = str;
				}
			}
			else if (id_from == id_to) { // вершины совпадают
					subpaths[std::make_tuple(id_from, id_to, k)] = "^";
			}
			else { // таких дуг нет и вершины не совпадают
					subpaths[std::make_tuple(id_from, id_to, k)] = "";
			}
		}
		// Строим по индукции (k != 0)
		else {

			// Высчитываем формулку
			if (subpaths.count(std::make_tuple(id_from, id_to, k - 1)) == 0) {
				KPathInduction(k - 1, id_from, id_to,  paths);
			}
			i_j = subpaths[std::make_tuple(id_from, id_to, k - 1)];

			if (subpaths.count(std::make_tuple(id_from, k, k - 1)) == 0) {
				KPathInduction(k - 1, id_from, k,  paths);
			}
			i_k = subpaths[std::make_tuple(id_from, k, k - 1)];
			if (i_k == "^") {
				i_k = "";
			}

			if (subpaths.count(std::make_tuple(k, k, k - 1)) == 0) {
				KPathInduction(k - 1, k, k, paths);
			}
			k_k = subpaths[std::make_tuple(k, k, k - 1)];
			if (k_k == "^") {
				k_k = "";
			}

			if (subpaths.count(std::make_tuple(k, id_to, k - 1)) == 0) {
				KPathInduction(k - 1, k, id_to,  paths);
			}
			k_j = subpaths[std::make_tuple(k, id_to, k - 1)];
			if (k_j == "^") {
				k_j = "";
			}
			
			// Формируем части регулярки

			std::string result;
			// если правая часть не пуста
			if ((i_k != "") && (k_j != "")) {
				if (k_k != "") {
					result = i_k + "((" + k_k + ")" + "+|^)" + k_j;
				}
				else {
					result = i_k + k_j;
				}
			}

			if ((!result.empty()) && (i_j != "")) {
				result = "(" + i_j + "|" + result + ")";
			}
			else if (result.empty()) {
				if (i_j != "") {
					result = i_j;
				}
				else {
					result = "";
				}
			}
			
			subpaths[std::make_tuple(id_from, id_to, k)] = result;
		}
	}

	void DFA::DFAproduct(DFA* first, DFA* second, int flag) {

		if ((first->GetStart() == nullptr) || (second->GetStart() == nullptr)) {
			throw std::exception("Automata must be initialized!");
		}

		// чистим за собой предыдущее состояние автомата
		for (auto it = states.begin(); it != states.end(); ++it) {
			delete* it;
		}
		states.clear();
		start = nullptr;
		alphabet.clear();
		accepting_states.clear();
		subpaths.clear();

		// сформировали алфавит
		std::set_intersection(first->alphabet.begin(), first->alphabet.end(), 
			second->alphabet.begin(), second->alphabet.end(), std::inserter(alphabet, alphabet.begin()));

		std::map<std::pair<State*, State*>, State*> dfa_prod; // соответствие пара исходных - новое состояние

		// сформировали состояния + позиции (просто пронумеровали)
		int id = 0;
		for (auto first_state : first->states) {
			for (auto second_state : second->states) {
				State* new_state = new State();
				new_state->positions.insert(id);
				dfa_prod[std::make_pair(first_state, second_state)] = new_state;
				++id;
			}
		}

		// сформировали старт
		start = dfa_prod[std::make_pair(first->start, second->start)];

		// формируем переходы + принимающие состояния
		for (auto& state : dfa_prod) {
			for (auto& symb : alphabet) {
				state.second->transitions[symb] = dfa_prod[std::make_pair(state.first.first->transitions[symb], state.first.second->transitions[symb])];
			}
			states.push_back(state.second); // добавляем состояния

			// устанавливаем принимающие состояния (флаг для гибкости)
			if (flag == 0) { // вызвана разность
				if ((first->accepting_states.contains(state.first.first)) && (!second->accepting_states.contains(state.first.second))) {
					accepting_states.insert(state.second);
				}
			}
			/*
			else if (flag == 1) { // вызвано дополнение

			}
			*/
		}
		DFAMinimization();
	}

	void DFA::DFAproduct(std::string first, std::string second, int flag) {
		DFA* first_automat = new DFA();
		first_automat->Compile(first);
		DFA* second_automat = new DFA();
		second_automat->Compile(second);

		DFAproduct(first_automat, second_automat, flag);
	}

	std::string DFA::Difference(DFA* first, DFA* second) {
		DFAproduct(first, second, 0);
		CreateDFAImg(".\\dfa_diff.png");
		return KPath();
	}

	std::string DFA::Difference(std::string first, std::string second) {
		DFA* first_dfa = new DFA();
		first_dfa->Compile(first);
		DFA* second_dfa = new DFA();
		second_dfa->Compile(second);
		return Difference(first_dfa, second_dfa);
	}

	std::string DFA::Complement(DFA* dfa) {

		// Формируем (алфавит)*
		std::set<std::string> alphabet = dfa->GetAlphabet();
		std::set<std::string> metacharacters = { "#", "|", ".", "+", "{", "}", "(", ")", "\\"};
		std::string universum = "(^|(";

		if (alphabet.size() == 0) {
			universum = "(^$)"; //because alphabet is empty
		}
		for (auto symb : alphabet) {
			if (metacharacters.find(symb) != metacharacters.end()) {
				symb = "#" + symb;
			}
			universum += symb + "|";
		}
		if (alphabet.size()) {
			universum.pop_back();
			universum += ")+)";
		}

		DFA* total_dfa = new DFA();
		total_dfa->Compile(universum);
		Difference(total_dfa, dfa);
		CreateDFAImg(".\\dfa_compl.png");
		return KPath();
	}

	std::string DFA::Complement(std::string str) {
		DFA* dfa = new DFA();
		dfa->Compile(str);
		return Complement(dfa);
	}

	// ----

}
