#include "Lexer.h"

namespace MyLib {
	
	bool Lexer::IsNumber(const std::string s) { // ��������, �������� �� ������ ������ (��� ������������� �����)
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) {
			++it;
		}
		return !s.empty() && it == s.end();
	}

	std::pair<int, int> Lexer::FindCLosestPair(const std::string &str, const int position) { // ������ ������ � ������� ������ ������									
		int last = position;														// ���� ������ �������� ��������� � ������, �� ��� ��������� ������
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

	void Lexer::ParseRe(std::string& str, std::vector<std::string>& vec) { // ������ ������ � ������ + ��������� �����

		for (int i = 0; i < int(str.size()); ++i) {
			std::string buf;
			std::pair<int, int> closest_pair;
			int difference = 0;
			char literal = str[i];

			// ������������ ������ ������
			switch (literal) {

			case '#': // ��������� ��� �������, ���� ��� ������������� ���������� 
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

			case '{': // ��������� ��� ������ ������� 

				// ���� ��� ���-�� ����������, �� � ������ ������������ ������ ���� "{�����,�����}" "{�����,+}", "{�����,}"
				// ����� ����������� ����� {}
				// ����� ��������� ������� ������� � ����������
				// ���� ���, �� ��������� ��� ����� �� isdigit
				// ���� ����, �� ��������� �����, ���� ��� ������ (������� �������), �� ����������, ���� �����, �� �������� ��� � ����� ��������.
				// {X}  {X, }  	{X, Y}
				// ���� ���� ���-�� �� ����� �� ���������, �� �� ������ ������� ����� ����� � ������

				closest_pair = FindCLosestPair(str, i); // ������� ���� ������
				difference = closest_pair.second - closest_pair.first;
				if (difference >= 2) { // ���� ����� �������� ���-�� ����
					++i;
					while (isdigit(str[i])) {
						buf.push_back(str[i]);
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
					else { // ��������� ����� ��� ���-�� ������
						i = closest_pair.first;
						vec.push_back(std::string(1, str[i]));

					}
				}
				else { // ���� ������ ����������� ������ �� ������� - ������ (��� ��� ������� {})
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

		// ������� ������������ ������ 
		while (str.find_first_of('(') != std::string::npos) { // ���� ���� ������������� ��������
			indx = str.find_first_of('(');
			int i = 1;
			while ((str[indx + i] != ':') && (isdigit(str[indx + i]))) { // ��������� ����� ������
				buf.push_back(str[indx + i]);
				++i;
			}

			if ((str[indx + i] == ':') && (!buf.empty())) {
				nums.push_back(buf); //  ��������� ������ � �������� ����� �������
			}
			buf.clear();
			str.erase(indx, 1);
		}

		std::set<std::string> tmp(nums.begin(), nums.end());
		if (tmp.size() != nums.size()) { // ���� ������ ������ ������� ���������
			throw std::exception("Invalid expression (capture group numbers)! Please enter the string again.");
		}
		tmp.clear();

		if (!nums.empty()) { // ����� ��� ����������
			for (auto it = nums.begin(); it != nums.end(); ++it) { // �������� �� ������� ����� (�.�. ������ ��� ��������� 1 ������)
				int len = (*it).size(); // ���� ����� ����� ����� �����, ���������� � ( � ������������� : ��� ���������� � /     
				size_t i = 0; // ������� ��� �������

				do { // ���� � ������� ������� ����� ����� (

					int j = 0; // ������� ��� �������� �� �����
					while ((i <= tokens.size() - 2) && (tokens[i] != "(") && (tokens[i] != "\\")) { // ������� ������, ��� �� �������������� ������ �������  
						++i;
					}
					++i;
					int flag = i; // ��������, ��� � ������� ���������� ������ �������

					while (((i <= tokens.size() - 2) && (std::string(1, (*it)[j]) == tokens[i]))) { // ���� ��� ������ ���������
						buf += tokens[i];
						++i;
						++j;
					}

					if ((buf == *it) && (!isdigit(tokens[i][0]))) { // ���� ������� � ������ �� ����� 
						if (tokens[flag - 1] == "(") {
							tokens[flag + len] = ":" + buf;
							tokens.erase(tokens.begin() + flag, tokens.begin() + flag + len);
						}
						else { // ���� ������� /
							tokens[flag - 1] += buf;
							tokens.erase(tokens.begin() + flag, tokens.begin() + flag + len);
						}
					}

					buf.clear();

				} while (i != tokens.size());
			}
		}

		if (std::find(tokens.begin(), tokens.end(), "\\") != tokens.end()) { // ����� ���������� \\ ������������ �� �� ����������
			throw std::exception("Invalid expression ( \\ )! Please enter the string again.");
		}
		
	}

	void Lexer::SetGroupLinks(std::vector<Node*>& capt_groups) {

		std::vector<Node*>::iterator g_it = capt_groups.begin();
		while (g_it != capt_groups.end()) { 

			if ((*g_it)->GetRight() != nullptr) { // ����� ����� ����������� ���� � ������� ������� 

				std::vector<Node*>::iterator current_node = g_it;
				while (current_node != capt_groups.end()) { // ���� ������� ����� ���� ������
					current_node = find_if(capt_groups.begin(), capt_groups.end(),
						[=](Node* p) {
							return ((p->GetType() == TypeOfNode::�apture_node)) && 
								((p->GetVal().second) == ("\\" + (*g_it)->GetVal().second));
						});

					if ((current_node != capt_groups.end()) && ((*g_it)->GetType() != TypeOfNode::unknown)) {
						(*current_node)->SetLeft((*g_it)->GetLeft()); // ��������� ������
						(*current_node)->SetRight((*g_it)->GetRight());

						(*current_node)->SetType(TypeOfNode::unknown);		 // �� ��� ������ ������?	
					}
				}
			}
			++g_it;
		}

		/*
		������ ��� ��� ���������� ���� ������ ������� ��� ��������, 
		��������� �� ������� ��������! */

		// ��� ��� ������, ��� � ��� ���������? ���� �� ����� �� ���������� ��� � ������� ���������
		// ���� ������ � ������� node*, �� �������� ������ ������������� �������� ��������� 
	}

	void Lexer::TreeAlgorithm(int br_count, std::vector<Node*>& syn_tree) {

		std::vector<Node*> capt_g; // ������ ��� �������� ���������� �� ������ �������

		// ������� ���������
		std::vector<Node*>::reverse_iterator l_bracket; // ����� ��������
	
		// ���� ���� ��������
		do {
			l_bracket = find_if(syn_tree.rbegin(), syn_tree.rend(), // ���� ��������� ������������� ������   
								[=](Node* p) {
								return (p->GetType() == TypeOfNode::unknown) && (p->GetVal().second == "(");
								});

			if ((*l_bracket.base())->GetVal().second == ")") { // ��������� �� ������������ �����, �������� ()
				throw std::exception("Invalid expression ( () )! Please enter the string again.");
			}

			int dist = std::distance(syn_tree.begin(), l_bracket.base()); // ������ �� ����� ������� �������� � ������� ������� ����� ������
			
			// ������� �-����
			for (auto it = l_bracket.base(); (*it)->GetVal().second != ")"; ++it) {
				std::string current_symb = (*it)->GetVal().second;
				if ((current_symb[0] == '\\') && (current_symb.size() > 1) && (*it)->GetType() == TypeOfNode::unknown) { // ���� ���������� ������ �������
					(*it)->SetType(TypeOfNode::�apture_node);
					capt_g.push_back(*it); // �������� ����� ������ ������� � ������
				}
				else if (((current_symb.size() == 1) && (!metacharacters.contains(current_symb[0])) ||
							((current_symb[0] == '#') && (metacharacters.contains(current_symb[1])))) && ((*it)->GetType() == TypeOfNode::unknown)) { // ���� ��� ������ �������� ��� #.
					(*it)->SetType(TypeOfNode::a_node);
				}
			}
			
			// ������� plus-����  
			auto it = l_bracket.base();
			std::string current_symb = "(";
			while (current_symb != ")") {
				current_symb = (*it)->GetVal().second;

				if (((it - 1) != syn_tree.end()) && (current_symb.find(",") != std::string::npos) && ((*(it - 1))->GetType() != TypeOfNode::unknown) && (current_symb.size() > 1)) { // ���� ��� ����� ���������� { }, ���������� -  ���, � ������� "+"

					(*it)->SetLeft(*(it - 1));
					(*(it - 1))->SetParent(*it);

					(*it)->SetType(TypeOfNode::plus_node);
					(*it)->SetVal(std::make_pair((*it)->GetVal().first, current_symb));

					it = syn_tree.erase(it - 1);

				}
				++it;
			}

			// ������� cat-���� 
			it -= std::distance(syn_tree.begin() + dist, it); // ����� ����� ������ ����� ���������, ������� ��� �� ����� ��������� // � ������ �������
			current_symb = "(";
			while (current_symb != ")") {
				current_symb = (*it)->GetVal().second;
				if (((it + 1) != syn_tree.end()) &&((*it)->GetType() != TypeOfNode::unknown) && ((*(it + 1))->GetType() != TypeOfNode::unknown)) { // ���� ������� � ��������� -  ����
					
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

			// ������� or-����
			it -= std::distance(syn_tree.begin() + dist, it);  // ����� ����� ������ ����� ���������, ������� ��� �� ����� ��������� // � ������ �������
			current_symb = "(";
			while (current_symb != ")") {
				current_symb = (*it)->GetVal().second;
				if (((it + 1) != syn_tree.end()) && ((it - 1) != syn_tree.end()) && ((*(it - 1))->GetType() != TypeOfNode::unknown) && ((*(it + 1))->GetType() != TypeOfNode::unknown) && (current_symb == "|")) { // ���� ���������� � ��������� - ����, � ������� |
					
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

			// ������� ���� ����� ������� 
			it -= std::distance(syn_tree.begin() + dist, it); // ����� ����� ������ ����� ���������, ������� ��� �� ����� ��������� // � ������ �������
			current_symb = "(";
			while (current_symb != ")") {
				current_symb = (*it)->GetVal().second;
				if ((current_symb[0] == ':') && (current_symb.size() > 1)) { // ������� - :���, � ��������� - ���, 
																																																			
					(*it)->SetRight(*(it + 1));
					(*(it + 1))->SetParent(*it);

					(*it)->SetType(TypeOfNode::�apture_node);
					(*it)->SetVal(std::make_pair((*it)->GetVal().first, current_symb.substr(1)));

					capt_g.push_back(*it); // �������� � ������ ���� ������ �������

					it = syn_tree.erase(it + 1);
				}
				else {
					++it;
				}
			}

			it = syn_tree.erase(it - std::distance(syn_tree.begin() + dist, it) - 1); // ���� ������� ����� ��������, �� ��������� ��� ��� ����� �������� ����� ������ (!!)
			it = syn_tree.erase(it + 1); // ���� ������� ������ �������� 
			
			--br_count;
		} while (br_count != 0);

		this->SetGroupLinks(capt_g); // ������������� ����������� ����� ��� ����� �������

		// ������� ��������� ������
		for (auto it = capt_g.begin(); it != capt_g.end(); ++it) {
			delete *it;
		}
		capt_g.clear();
	}

	void Lexer::Re2Tree(std::string& str) { // ������ ������, ������ ���������

		str = '(' + str + '$' + ')';
		while (str.find(' ') != std::string::npos) { // ������ ��� �������, ����� ��� ��� �� ������ (�� �������� �������-������)
			str.erase(str.find(' '), 1);
		}

		std::vector<std::string> tok_vec; // ������, ���� ����� ���������� ������ (�������� ��� ����� ���������� ������� �������)
		ParseRe(str, tok_vec); // ������ ������ � ������
		int open_br_count = count(tok_vec.cbegin(), tok_vec.cend(), "(");
		if (open_br_count != count(tok_vec.cbegin(), tok_vec.cend(), ")")) { // ��������� �� ������������ ����� 
			throw std::exception("Invalid expression! Please enter the string again.");
		}

		std::vector<std::string> group_nums; // ����� ����� ��� ������ ��� ����� �������
		this->CaptGroupDetecting(str, group_nums, tok_vec); // ��� ���������� �� ������� ������ ���� ����� ������� + ���������� ������. ������
		group_nums.clear();
		
		std::vector<Node*> syn_tree; // ������ ����� ��� ���������� �� 
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
		gvRenderFilename(gvc, g, "png", "C:\\Users\\Asus\\Desktop\\������ ���������\\Laboratory2\\MyLib\\MyLib\\test.png");
		gvFreeLayout(gvc, g);
		agclose(g);
		gvFreeContext(gvc);
	}
};

