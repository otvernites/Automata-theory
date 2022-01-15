#pragma once

#include "Lexer.h"

namespace MyLib {
	/*
	typedef struct Tran {
		std::string symb; // символ, по которому осуществляется переход
		State* start;
		State* end;

		Tran(std::string s = "", State* from = nullptr, State* to = nullptr):
													symb(s), start(from), end(to) {}
	};
	*/

	// состояние автомата
	typedef struct State {
		std::map<std::string, State*> transitions; // соответствие: каждый символ алфавита - в какое состояние перейдет (по этому символу) (стрелочки)
		std::set<int> positions; // множ-во позиций, которые могут быть следующими (лежат в кругляшке)
	};

	class DFA: public Lexer {
	private:
		State* start; // стартовое состояние
		std::set<std::string> alphabet; // символы алфавита

		std::vector<State*> states; // множество состояний автомата (произв порядок)
		std::set<State*> accepting_states; // множ-во принимающих состояний (произв)

		// ----
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
		// ----
		
		// для функции печати
		void PrintState(std::ostringstream& buffer);
	public:
		DFA(Node* r = nullptr, State* s = nullptr): Lexer(r), start(s) {}

		// Для одновременного построения множеств 
		void TreeTraversal(Node* root, std::vector<std::set<int>>& follow_pos);

		// Создание состояний
		void CreatingStates(std::unordered_map<int, std::string>& map, std::vector<std::set<int>>& follow_pos);

		// Минимизация ДКА 
		void DFAMinimization();

		// Обертка для алгоритма преобразования
		void Re2DFA();

		// Изображение DFA
		void CreateDFAImg();
	};
}