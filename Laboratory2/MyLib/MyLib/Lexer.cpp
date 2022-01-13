#include "Lexer.h"

namespace MyLib {
	
	bool Lexer::IsNumber(const std::string s) { // проверка, является ли строка числом (для положительный целых)
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) {
			++it;
		}
		return !s.empty() && it == s.end();
	}

	std::pair<int, int> Lexer::FindCLosestPair(const std::string &str, const int position) { // подаем строку и позицию первой скобки									
		int last = position;														// если второй аргумент совпадает с первым, то это одиночный символ
		while (last != str.size()) {
			if (str[last] == ')' && str[position] == '(' || str[last] == '}' && str[position] == '{') {
				break;
			}
			++last;
		}
		if ((str[last] != ')') && (str[last] != '}')) {
			last = position;
		}
		return std::make_pair(position, last);
	}

	void Lexer::ParseRe(std::string& str, std::vector<std::string>& vec) { // парсим токены в вектор + валидация ввода

		for (int i = 0; i < int(str.size()); ++i) {
			std::string buf;
			std::pair<int, int> closest_pair;
			int difference = 0;
			char literal = str[i];

			// обрабатываем особые случаи
			switch (literal) {

			case '#': // добавляем оба символа, если это экранированый метасимвол 
				if (metacharacters.contains(str[i+1])) {
					buf.push_back(str[i]);
					buf.push_back(str[i+1]);
					vec.push_back(buf);
					buf.clear();
					++i;
				}
				else {
					throw std::exception("Invalid expression (#)! Please enter the string again.");
				}
				break;

			case '{': // валидация для группы повтора 

				// если это кол-во повторений, то в вектор отправляется строка вида "{число,число}" "{число,+}", "{число,}"
				// можно попробовать найти {}
				// потом проверчем наличие запятой в промежутке
				// если нет, то проверяем все число на isdigit
				// если есть, то проверяем число, если там ничего (включая пробелы), то бесконечно, если число, то сравнить его с левой границей.
				// {X}  {X, }  	{X, Y}
				// если хоть что-то из этого не совпадает, то по одному символу сразу пушим в вектор

				closest_pair = FindCLosestPair(str, i); // находим пару скобок
				difference = closest_pair.second - closest_pair.first;
				if (difference >= 2) { // если между скобками что-то есть
					++i;
					while (isdigit(str[i])) {
						buf.push_back(str[i]);
						if (std::stoi(buf) == 0) {
							throw std::exception("Invalid expression ({0, })! Please enter the string again.");
						}
						buf.push_back(',');
						++i;
					}
					if (i == closest_pair.second) { // {x}
						vec.push_back(buf);
						buf.clear();
					}
					else if (str[i] == ',' && str[i+1] == '}') { // {x,}
						buf.push_back('+');
						vec.push_back(buf);
						buf.clear();
						++i;
					}
					else if ((IsNumber(str.substr(i + 1, closest_pair.second - i - 1))) && 
						(std::stoi(str.substr(closest_pair.first + 1, i - closest_pair.first - 1)) < std::stoi(str.substr(i + 1, closest_pair.second - i - 1)))) { // {x,y}  
						++i;
						while (i != closest_pair.second) {
							buf.push_back(str[i]);
							++i;
						}
						vec.push_back(buf);
						buf.clear();
					}
					else { // встретили букву или что-то другое
						i = closest_pair.first;
						vec.push_back(std::string(1, str[i]));

					}
				}
				else { // если второй закрывающей скобки не нашлось - ошибка (или это символы {})
					throw std::exception("Invalid expression ({})! Please enter the string again.");
				}
				break;

			case '+':
				if (str[i - 1] == '(') {
					throw std::exception("Invalid expression (+)! Please enter the string again.");
					break;
				}
				else {
					vec.push_back("1,+");
					break;
				}

			case '.':
				if ((str[i-1] == '(') || (str[i + 1] == ')')) {
					throw std::exception("Invalid expression (.)! Please enter the string again.");
					break;
				}

			case '|':
				if ((str[i - 1] == '(') || (str[i + 1] == ')')) {
					throw std::exception("Invalid expression (|)! Please enter the string again.");
					break;
				}

			default:
				vec.push_back(std::string(1, literal));
			}
		}
	}

	void Lexer::CaptGroupDetecting(std::string str, std::vector<std::string>& nums, std::vector<std::string>& tokens) {

		std::string buf;
		size_t indx = 0;

		// находим существующие номера 
		while (str.find_first_of('(') != std::string::npos) { // пока есть открывающиеся скобочки
			indx = str.find_first_of('(');
			int i = 1;
			while ((str[indx + i] != ':') && (isdigit(str[indx + i]))) { // считываем номер группы
				buf.push_back(str[indx + i]);
				++i;
			}

			if ((str[indx + i] == ':') && (!buf.empty())) {
				nums.push_back(buf); //  заполняем вектор с номерами групп захвата
			}
			buf.clear();
			str.erase(indx, 1);
		}

		std::set<std::string> tmp(nums.begin(), nums.end());
		if (tmp.size() != nums.size()) { // если разные группы названы одинаково
			throw std::exception("Invalid expression (capture group numbers)! Please enter the string again.");
		}
		tmp.clear();

		if (!nums.empty()) { // иначе это бесполезно
			for (auto it = nums.begin(); it != nums.end(); ++it) { // итерация по номерам групп (т.е. каждый раз проверяем 1 группу)
				int len = (*it).size(); // надо найти числа такой длины, начинаются с ( и заканчиваются : ИЛИ начинаются с /     
				size_t i = 0; // счетчик для вектора

				do { // пока в векторе находим такую цифру (

					int j = 0; // счетчик для итерации по числу
					while ((i <= tokens.size() - 2) && (tokens[i] != "(") && (tokens[i] != "\\")) { // находим индекс, где мб предполагаться группа захвата  
						++i;
					}
					++i;
					int flag = i; // ориентир, где в векторе начинается группа захвата

					while (((i <= tokens.size() - 2) && (std::string(1, (*it)[j]) == tokens[i]))) { // если они начали совпадать
						buf += tokens[i];
						++i;
						++j;
					}

					if ((buf == *it) && (!isdigit(tokens[i][0]))) { // если совпали и дальше не число 
						if (tokens[flag - 1] == "(") {
							tokens[flag + len] = ":" + buf;
							tokens.erase(tokens.begin() + flag, tokens.begin() + flag + len);
						}
						else { // если сначала /
							tokens[flag - 1] += buf;
							tokens.erase(tokens.begin() + flag, tokens.begin() + flag + len);
						}
					}

					buf.clear();

				} while (i != tokens.size());
			}
		}

		if (std::find(tokens.begin(), tokens.end(), "\\") != tokens.end()) { // когда метасимвол \\ используется не по назначению
			throw std::exception("Invalid expression ( \\ )! Please enter the string again.");
		}
	}

	Node* Lexer::CopyBranch(Node* node) {
		Node* child_left = nullptr;
		Node* child_right = nullptr;
		if (node->GetLeft()) {
			child_left = CopyBranch(node->GetLeft());
		}
		if (node->GetRight()) {
			child_right = CopyBranch(node->GetRight());
		}
		Node* ptr = new Node(*node);
		ptr->SetLeft(child_left);
		if (child_left) {
			child_left->SetParent(ptr);
		}	
		ptr->SetRight(child_right);
		if (child_right) {
			child_right->SetParent(ptr);
		}
		return ptr;
	}

	void Lexer::GroupPaste(std::vector<Node*>& capt_groups) {

		std::vector<Node*>::iterator g_it = capt_groups.begin();
		while (g_it != capt_groups.end()) { 

			if ((*g_it)->GetRight() != nullptr) { // нашли здесь определение узла с группой захвата 

				std::vector<Node*>::iterator it; 
				
				for (it = capt_groups.begin(); it != capt_groups.end(); it++) {
					if ((*it)->GetVal().second == ("\\" + (*g_it)->GetVal().second)) {
						//копирование
						(*it)->SetRight(CopyBranch((*g_it)->GetRight()));
					}
				}
			}
			++g_it;
		}
	}	

	void Lexer::RepeatPaste(std::vector<Node*>& repeat_g) {

		std::vector<Node*>::iterator g_it;
		for (g_it = repeat_g.begin(); g_it != repeat_g.end(); ++g_it) {
			std::string str = (*g_it)->GetVal().second;
			int value = std::atoi(str.c_str()); //тут лежит первое число
			// {X}
			if (str[str.size() - 1] == ',') {
				if ((*g_it)->GetParent()->GetLeft() == *g_it) { // если левое поддерево
					if (value == 1) { // если повторяем один раз, то просто новые связи
						(*g_it)->GetParent()->SetLeft((*g_it)->GetLeft()); // родителю установила
						(*g_it)->GetLeft()->SetParent((*g_it)->GetParent()); // потомку установила
						delete* (g_it); //
					}
					else {
						Node* tmp = (*g_it)->GetParent();
						while (value != 1) {
							Node* oper = new Node("*", TypeOfNode::and_node);
							oper->SetParent(tmp);
							tmp->SetLeft(oper);
							oper->SetRight(CopyBranch((*g_it)->GetLeft()));
							--value;
							tmp = oper;
						}
						tmp->SetLeft(CopyBranch((*g_it)->GetLeft()));
						delete* g_it; // 
					}
				}
				else if ((*g_it)->GetParent()->GetRight() == *g_it) { // если правое поддерево
					if (value == 1) { // если повторяем один раз, то просто новые связи
						(*g_it)->GetParent()->SetRight((*g_it)->GetRight()); // родителю установила
						(*g_it)->GetRight()->SetParent((*g_it)->GetParent()); // потомку установила
						delete* g_it; //
					}
					else {
						Node* tmp = (*g_it)->GetParent();
						while (value != 1) {
							Node* oper = new Node("*", TypeOfNode::and_node);
							oper->SetParent(tmp);
							tmp->SetRight(oper);
							oper->SetRight(CopyBranch((*g_it)->GetLeft()));
							--value;
							tmp = oper;
						}
						tmp->SetLeft(CopyBranch((*g_it)->GetLeft()));
						delete* g_it; // 
					}
				}
			}
			// {X, +}
			else if (str[str.size() - 1] == '+') {
				if ((*g_it)->GetParent()->GetLeft() == *g_it) { // если левое поддерево
					if (value != 1) { // 
						Node* tmp = (*g_it)->GetParent();
						Node* oper = new Node("*", TypeOfNode::and_node);
						oper->SetParent(tmp);
						tmp->SetLeft(oper);
						oper->SetRight(CopyBranch((*g_it)));
						oper->GetRight()->SetVal(std::make_pair(-1, "+"));
						--value;
						tmp = oper;

						while (value != 1) {
							oper = new Node("*", TypeOfNode::and_node);
							oper->SetParent(tmp);
							tmp->SetLeft(oper);
							oper->SetRight(CopyBranch((*g_it)->GetLeft()));
							--value;
							tmp = oper;
						}
						tmp->SetLeft(CopyBranch((*g_it)->GetLeft()));
						delete* g_it; // 
					}
					else { // если повторяем один раз, то просто меняем вид {1,+} на +
						(*g_it)->SetVal(std::make_pair(-1, "+"));
					}
				}
				else if ((*g_it)->GetParent()->GetRight() == *g_it) { // если правое поддерево
					if (value != 1) { // если повторяем один раз, то просто оставляем
						Node* tmp = (*g_it)->GetParent();
						Node* oper = new Node("*", TypeOfNode::and_node);
						oper->SetParent(tmp);
						tmp->SetRight(oper);
						oper->SetRight(CopyBranch((*g_it)));
						oper->GetRight()->SetVal(std::make_pair(-1, "+"));
						--value;
						tmp = oper;
						
						while (value != 1) {
							oper = new Node("*", TypeOfNode::and_node);
							oper->SetParent(tmp);
							tmp->SetRight(oper);
							oper->SetRight(CopyBranch((*g_it)->GetLeft()));
							--value;
							tmp = oper;
						}
						tmp->SetLeft(CopyBranch((*g_it)->GetLeft()));
						delete* g_it; // 
					}
					else { // если повторяем один раз, то просто меняем вид {1,+} на +
						(*g_it)->SetVal(std::make_pair(-1, "+"));
					}
				}
			}
						/*// {X, Y}
			else {

			}
			*/
			//++g_it;

		}
	}

	void Lexer::TreeAlgorithm(int br_count, std::vector<Node*>& syn_tree) {

		std::vector<Node*> capt_g; // вектор для хранения указателей на группы захвата
		std::vector<Node*> repeat_g; // вектор для хранения указателей на группы повтора

		// следуем алгоритму
		std::vector<Node*>::reverse_iterator l_bracket; // левая скобочка
	
		// пока есть скобочки
		do {
			l_bracket = find_if(syn_tree.rbegin(), syn_tree.rend(), // ищем последнюю открывающуюся скобку   
								[=](Node* p) {
								return (p->GetType() == TypeOfNode::unknown) && (p->GetVal().second == "(");
								});

			if ((*l_bracket.base())->GetVal().second == ")") { // проверяем на корректность ввода, ситуация ()
				throw std::exception("Invalid expression ( () )! Please enter the string again.");
			}

			int dist = std::distance(syn_tree.begin(), l_bracket.base()); // доступ по этому индексу приведет к первому символу после скобки
			
			// создаем а-ноды
			for (auto it = l_bracket.base(); (*it)->GetVal().second != ")"; ++it) {
				std::string current_symb = (*it)->GetVal().second;
				if ((current_symb[0] == '\\') && (current_symb.size() > 1) && (*it)->GetType() == TypeOfNode::unknown) { // если вызывается группа захвата
					(*it)->SetType(TypeOfNode::capture_node);
					capt_g.push_back(*it); // добавили вызов группы захвата в вектор
				}
				else if (((current_symb.size() == 1) && (!metacharacters.contains(current_symb[0])) ||
							((current_symb[0] == '#') && (metacharacters.contains(current_symb[1])))) && ((*it)->GetType() == TypeOfNode::unknown)) { // если это символ алфавита или #.
					(*it)->SetType(TypeOfNode::a_node);
				}
			}
			
			// создаем plus-ноды  
			auto it = l_bracket.base();
			std::string current_symb = "(";
			while (current_symb != ")") {
				current_symb = (*it)->GetVal().second;

				if (((it - 1) != syn_tree.end()) && (current_symb.find(",") != std::string::npos) && ((*(it - 1))->GetType() != TypeOfNode::unknown) && (current_symb.size() > 1)) { // если это токен повторений { }, предыдущий -  нод, а текущий "+"

					(*it)->SetLeft(*(it - 1));
					(*(it - 1))->SetParent(*it);

					(*it)->SetType(TypeOfNode::plus_node);
					(*it)->SetVal(std::make_pair((*it)->GetVal().first, current_symb));

					repeat_g.push_back(*it);

					it = syn_tree.erase(it - 1);

				}
				++it;
			}

			// создаем cat-ноды 
			it -= std::distance(syn_tree.begin() + dist, it); // здесь левая скобка может сломаться, поэтому ищу ее новое положение // в старом векторе
			current_symb = "(";
			while (current_symb != ")") {
				current_symb = (*it)->GetVal().second;
				if (((it + 1) != syn_tree.end()) &&((*it)->GetType() != TypeOfNode::unknown) && ((*(it + 1))->GetType() != TypeOfNode::unknown)) { // если текущий и следующий -  ноды
					
					Node* node = new Node("*", TypeOfNode::and_node);
					node->SetLeft(*it);
					node->SetRight(*(it + 1));
					(*it)->SetParent(node);
					(*(it + 1))->SetParent(node);

					it = syn_tree.insert(it, node);
					it = syn_tree.erase(it + 1, it + 3);
					--it;
				}
				else {
					++it;
				}
			}

			// создаем or-ноды
			it -= std::distance(syn_tree.begin() + dist, it);  // здесь левая скобка может сломаться, поэтому ищу ее новое положение // в старом векторе
			current_symb = "(";
			while (current_symb != ")") {
				current_symb = (*it)->GetVal().second;
				if (((it + 1) != syn_tree.end()) && ((it - 1) != syn_tree.end()) && ((*(it - 1))->GetType() != TypeOfNode::unknown) && ((*(it + 1))->GetType() != TypeOfNode::unknown) && (current_symb == "|")) { // если предыдущий и следующий - ноды, а текущий |
					
					(*it)->SetLeft(*(it - 1));
					(*it)->SetRight(*(it + 1));
					(*(it - 1))->SetParent(*it);
					(*(it + 1))->SetParent(*it);

					(*it)->SetType(TypeOfNode::or_node);
					(*it)->SetVal(std::make_pair((*it)->GetVal().first, current_symb));

					it = syn_tree.erase(it - 1);
					it = syn_tree.erase(it + 1);
				}
				else {
					++it;
				}
			}

			// создать ноды групп захвата 
			it -= std::distance(syn_tree.begin() + dist, it); // здесь левая скобка может сломаться, поэтому ищу ее новое положение // в старом векторе
			current_symb = "(";
			while (current_symb != ")") {
				current_symb = (*it)->GetVal().second;
				if ((current_symb[0] == ':') && (current_symb.size() > 1)) { // текущий - :ара, а следующий - нод, 
																																																			
					(*it)->SetRight(*(it + 1));
					(*(it + 1))->SetParent(*it);

					(*it)->SetType(TypeOfNode::capture_node);
					(*it)->SetVal(std::make_pair((*it)->GetVal().first, current_symb.substr(1)));

					capt_g.push_back(*it); // добавили в вектор ноду группы захвата

					it = syn_tree.erase(it + 1);
				}
				else {
					++it;
				}
			}

			it = syn_tree.erase(it - std::distance(syn_tree.begin() + dist, it) - 1); // надо удалить левую скобочку, но высчитать для нее новое значение левой скобки (!!)
			it = syn_tree.erase(it + 1); // надо удалить правую скобочку 
			
			--br_count;
		} while (br_count != 0);

		this->GroupPaste(capt_g); // дорисовываем группы захвата при вызове
		capt_g.clear(); // очищаем временный вектор

		this->RepeatPaste(repeat_g); // дорисовываем повторения при вызове
		repeat_g.clear();
	}

	void Lexer::Re2Tree(std::string& str) { // строим дерево, следуя алгоритму

		str = '(' + str + '$' + ')';
		while (str.find(' ') != std::string::npos) { // удалим все пробелы, чтобы они нам не мешали (их заменяет эпсилон-символ)
			str.erase(str.find(' '), 1);
		}

		std::vector<std::string> tok_vec; // вектор, куда будем отправлять токены (очистить его после заполнения второго вектора)
		ParseRe(str, tok_vec); // парсим токены в вектор
		int open_br_count = count(tok_vec.cbegin(), tok_vec.cend(), "(");
		if (open_br_count != count(tok_vec.cbegin(), tok_vec.cend(), ")")) { // проверяем на корректность ввода 
			throw std::exception("Invalid expression! Please enter the string again.");
		}

		std::vector<std::string> group_nums; // здесь лежат все номера для групп захвата
		this->CaptGroupDetecting(str, group_nums, tok_vec); // для построения СД получим номера всех групп захвата + объединяем разъед. токены
		group_nums.clear();
		
		std::vector<Node*> syn_tree; // вектор нодов для построения СД 
		for (auto it = tok_vec.begin(); it != tok_vec.end(); ++it) {
			Node* node = new Node(*it, TypeOfNode::unknown);
			syn_tree.push_back(node);
		}
		tok_vec.clear();

		this->TreeAlgorithm(open_br_count, syn_tree); 
		this->SetRoot(syn_tree[0]);
	}

	void Lexer::PrintNode(const Node* root, std::ostringstream& buffer, int id) {

		if (root == nullptr) {
			return;
		}

		if (root->GetLeft() != nullptr) {
			buffer << id << " -> " << id * 2 << ";\n";
		}

		if (root->GetRight() != nullptr) {
			buffer << id << " -> " << id * 2 + 1 << ";\n";
		}
		
		buffer << id << " [label=" << "\"" << id << " " << *root << "\"];\n";

		PrintNode(root->GetLeft(), buffer, id * 2);
		PrintNode(root->GetRight(), buffer, id * 2 + 1);
	}

	void Lexer::CreateTreeImg() {
		
		std::ostringstream buffer("", std::ios_base::ate);

		buffer << "digraph G {\n";
		PrintNode(this->GetRoot(), buffer, 1);
		buffer << "}";

		Agraph_t* g = agmemread(buffer.str().c_str());
		GVC_t* gvc = gvContext();
		gvLayout(gvc, g, "dot");
		gvRenderFilename(gvc, g, "png", ".\\test.png");
		gvFreeLayout(gvc, g);
		agclose(g);
		gvFreeContext(gvc);
	}
};

