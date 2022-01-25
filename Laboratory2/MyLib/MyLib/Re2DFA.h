#pragma once

#include "Lexer.h"

namespace MyLib {

	// состояние автомата
	struct State {
		std::map<std::string, State*> transitions; // соответствие: каждый символ алфавита - в какое состояние перейдет (по этому символу) (стрелочки)
		std::set<int> positions; // множ-во позиций, которые могут быть следующими (лежат в кругляшке)
	};

	// автомат
	class DFA: public Lexer {
	private:
		State* start; // стартовое состояние
		std::set<std::string> alphabet; // символы алфавита

		std::vector<State*> states; // множество состояний автомата (произв порядок)
		std::set<State*> accepting_states; // множ-во принимающих состояний (произв)

		// ----------
		// Нумерация символов алфавита
		void Numbering(Node* node, std::unordered_map<int, std::string>& map);

		// Проверить обнуляемость 
		void FindNullable(Node* node);

		// Построить множества первых позиций
		void FindFirst(Node* node);

		// Построить множество последних позиций
		void FindLast(Node* node);

		// Построить множество следующих позиций
		void FindFollowPos(Node* node, std::vector<std::set<int>>& follow_pos);

		// ----------
		// Для проверки, принадлежат ли эти состояния одной группе или нет
		bool IsDiffGroups(std::vector<std::set<State*>>& groups, State* first_state, State* second_state, std::string symb);

		// Для функции печати
		void PrintState(std::ostringstream& buffer);

		// Геттер для стартового состояния
		State* GetStart() const {
			return start;
		}

	protected:
		// ----------
		// Для одновременного построения множеств 
		void TreeTraversal(Node* root, std::vector<std::set<int>>& follow_pos);

		// Создание ДКА
		void CreatingStates(std::unordered_map<int, std::string>& map, std::vector<std::set<int>>& follow_pos);

		// Минимизация ДКА 
		void DFAMinimization();

		// Изображение DFA
		void CreateDFAImg(const char* f_name);

	public:
		DFA(Node* r = nullptr, State* s = nullptr): Lexer(r), start(s) {}

		// Построение минДКА из РВ
		DFA& Compile(std::string& str);

		// Для регулярного выражения
		std::vector<std::string> FindAll(std::string& sample, std::string& re);

		// Для скомпилированного автомата
		std::vector<std::string> FindAll(std::string& sample);
	};
}