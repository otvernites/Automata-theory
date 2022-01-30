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

		std::map<std::tuple<int, int, int>, std::string> subpaths; // k-пути для всех состояний, по которым строится ДКА (откуда, куда, промежуточное состояние) 
		std::unordered_map<int, std::set<int>> pos_table; // таблица позиций для соответствующих групп захвата

		std::unordered_map<int, std::vector<std::string>> group_results; // результаты для групп захвата
		std::unordered_map<int, std::string> conformity; // соответствие позиция-символ алфавита (нужна для построения состояний)
		// ----------
		// Нумерация символов алфавита
		void Numbering(Node* node, std::vector<int> stack_capture_groups);

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

		//Получить информацию о строках, захваченных группами захвата
		void Analyze_path(std::string correct_str); 

		
		// Геттер для стартового состояния
		State* GetStart() const {
			return start;
		}

		// Геттер для всех состояний
		std::vector<State*> GetStates() const {           
			return states;
		}

		// Геттер для принимающих состояний
		std::set<State*> GetAcceptStates() const {
			return accepting_states;
		}

		std::set<std::string> GetAlphabet() const {
			return alphabet;
		}
			
		std::unordered_map<int, std::set<int>> GetPosTable() const {
			return pos_table;
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

		// ----------
		// Шаги индукции для k-пути
		void KPathInduction(int k, int id_from, int id_to, std::map<std::pair<int, int>, std::vector<std::string>> paths);

		// Произведение автоматов (скомп. автоматы)
		void DFAproduct(DFA* first, DFA* second, int flag);

		// Произведение автоматов (рег. выражения)
		void DFAproduct(std::string first, std::string second, int flag);


	public:
		DFA(Node* r = nullptr, State* s = nullptr): Lexer(r), start(s) {}

		std::vector<std::string> operator[] (int idx) {
				return group_results[idx];
		}
		// Геттер для кол-ва минимизированных состояний
		int GetMinStates() const;

		// Построение минДКА из РВ
		DFA* Compile(std::string str);

		// Для регулярного выражения поиск всех непересекающизся вхождений (без доступа к гр.з.)
		std::vector<std::string> FindAll(std::string re, std::string sample);

		// Для скомпилированного автомата поиск всех непересекающизся вхождений (без доступа к гр.з.)
		std::vector<std::string> FindAll(std::string sample);

		// ДКА в регулярку
		std::string KPath(); 

		// Разность языков (скомп. автоматы)
		std::string Difference(DFA* first, DFA* second);

		// Разность языков (рег. выражения)
		std::string Difference(std::string first, std::string second);
		
		// Дополнение языка (скомп. автомат)
		std::string Complement(DFA* dfa);

		// Дополнение языка (рег. выражение)
		std::string Complement(std::string str);

	};
}