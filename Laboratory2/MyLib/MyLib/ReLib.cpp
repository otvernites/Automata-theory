/*
#include "ReLib.h"

namespace MyLib {

	std::vector<std::string> ReLib::FindAll(std::string sample) {

		// ������������� ������
		std::set<std::string> metacharacters = { "#", "|", ".", "+", "{", "}", "(", ")", "\\" };
		auto str_it = sample.begin();
		while (str_it != sample.end()) {
			if (metacharacters.contains(std::string(1, *str_it))) {
				str_it = sample.insert(str_it, '#');
				++str_it;
			}
			++str_it;
		}

		State* current_state = GetStart();
		if (current_state == nullptr) {
			throw std::exception("The automate needs to be initialized!");
		}
		std::vector<std::string> results;

		// ���� ��������� ��������� � ����������� 
		if (accepting_states.contains(current_state)) {
			results.push_back("");
		}

		std::string correct_str, tmp_str;
		size_t i = 0;
		size_t potential_next_id = -1; // ����, ����� �� �������� ������ ��� ��������

		// �������� ��������� ������
		while (i < sample.size()) {

			std::string symb;
			if (sample.substr(i, 1) == "#") {
				symb = sample.substr(i, 2);
				++i;
			}
			else {
				symb = sample.substr(i, 1);
			}

			// �������� ������ �� ��������� �����������
			if (current_state->transitions.count(symb) > 0) { // ���� ���������� ������� �� ����� �������
				current_state = current_state->transitions[symb];

				// ������������� ����. ������������� ������
				if ((current_state != GetStart()) && (potential_next_id == -1)) {
					potential_next_id = i + 1;
				}

				if (current_state->positions.size() > 0) { // ���� ����. ��������� ����������
					tmp_str.push_back(sample.at(i));
				}
				else {  // ���� ����. ��������� - �������
					if (correct_str.size() > 0) { // ���� ���-�� ������ �������, ���������
						results.push_back(correct_str);
						potential_next_id = -1;
						correct_str.clear();
						tmp_str.clear();
						current_state = GetStart();
						--i;
					}
					else { // ������ �� �������
						i = potential_next_id;
						potential_next_id = -1;
						current_state = GetStart();
						tmp_str.clear();
						continue;
					}
				}
			}
			else {  // �� ����������
				if (correct_str.size() > 0) { // ���� ���-�� ������ �������, ���������
					results.push_back(correct_str);
					correct_str.clear();
					potential_next_id = -1;
					tmp_str.clear();
					current_state = GetStart();
					--i;
				}
				else { // ������ �� �������
					current_state = GetStart();
					tmp_str.clear();
				}
			}

			// ����� ������ ���������, �.�. ������ � ����������� ���������
			if (accepting_states.contains(current_state)) {
				correct_str = tmp_str;
			}
			++i;

			// ��������� ��������, ����� ������ ����� �� �����, �� ��������� ��� ����� �������
			if ((i == sample.size()) && (correct_str.size() == 0) && (potential_next_id < sample.size())) {
				i = potential_next_id;
				current_state = GetStart();
				tmp_str.clear();
				potential_next_id = -1;
			}
		}

		// ��������� ��������� � ������
		if (correct_str.size() > 0) {
			results.push_back(correct_str);
			correct_str.clear();
		}

		// �������� ����������
		sort(results.begin(), results.end());
		results.resize(unique(results.begin(), results.end()) - results.begin());

		return results;
	}

	std::vector<std::string> ReLib::FindAll(std::string re, std::string sample) {
		Compile(re);
		return FindAll(sample);
	}

	std::string ReLib::KPath() {
		std::set<std::string> metacharacters = { "#", "|", ".", "+", "{", "}", "(", ")", "\\" };

		subpaths.clear(); // ������� ��������� ��� ������������� ��������� ��������
		std::string re; // ��������� ��� ��� (�� ���� ���������)

		std::vector<State*> states = GetStates(); // ��������� ��������
		std::map<State*, int> numb_states; // ��������� �������� (��������� - ��� �����)
		std::map<std::pair<int, int>, std::vector<std::string>> paths; // ��� k = 0, ����� �������-����� ����� ���������

		// �������� ��������� 
		int i = 1;
		for (auto st : states) {
			numb_states[st] = i;
			++i;
		}
		int k = numb_states.size(); // ����� ���������� ���������

		// ������ ���� ��� k = 0
		for (auto st : states) {
			for (auto& symb : GetAlphabet()) {
				int from = numb_states[st];
				int to = numb_states[st->transitions[symb]];
				paths[std::make_pair(from, to)].push_back(symb);
			}
		}

		// ���� �������� 
		State* start_state = GetStart();
		for (auto st : GetAcceptStates()) {
			KPathInduction(k, numb_states[start_state], numb_states[st], paths);
			re += subpaths[std::make_tuple(numb_states[start_state], numb_states[st], k)] + "|";
		}
		if (!re.empty()) {
			re.pop_back();
		}

		return re;
	}

	void ReLib::KPathInduction(int k, int id_from, int id_to, std::map<std::pair<int, int>, std::vector<std::string>> paths) {
		std::string i_j, i_k, k_k, k_j;
		std::set<std::string> metacharacters = { "#", "|", ".", "+", "{", "}", "(", ")", "\\" };

		// ������ �����
		if (k == 0) {
			std::string str;
			// ����� (k = 0)
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
			else if (id_from == id_to) { // ������� ���������
				subpaths[std::make_tuple(id_from, id_to, k)] = "^";
			}
			else { // ����� ��� ��� � ������� �� ���������
				subpaths[std::make_tuple(id_from, id_to, k)] = "";
			}
		}
		// ������ �� �������� (k != 0)
		else {

			// ����������� ��������
			if (subpaths.count(std::make_tuple(id_from, id_to, k - 1)) == 0) {
				KPathInduction(k - 1, id_from, id_to, paths);
			}
			i_j = subpaths[std::make_tuple(id_from, id_to, k - 1)];

			if (subpaths.count(std::make_tuple(id_from, k, k - 1)) == 0) {
				KPathInduction(k - 1, id_from, k, paths);
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
				KPathInduction(k - 1, k, id_to, paths);
			}
			k_j = subpaths[std::make_tuple(k, id_to, k - 1)];
			if (k_j == "^") {
				k_j = "";
			}

			// ��������� ����� ���������

			std::string result;
			// ���� ������ ����� �� �����
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
}
*/