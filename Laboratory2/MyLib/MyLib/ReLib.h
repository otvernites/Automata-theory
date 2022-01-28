/*
#pragma once
#include "Re2DFA.h"

namespace MyLib {
	// автомат
	class ReLib : public DFA {
	private:
		std::map<std::tuple<int, int, int>, std::string> subpaths; // k-пути для всех состояний, по которым строится ДКА (откуда, куда, промежуточное состояние) !!!!!!!!!!!

	protected:
		// ----------
		void KPathInduction(int k, int id_from, int id_to, std::map<std::pair<int, int>, std::vector<std::string>> paths); // !!!!!!!!!!

	public:
		ReLib() : DFA() {}

		// Для регулярного выражения поиск всех непересекающизся вхождений (без доступа к гр.з.)
		std::vector<std::string> FindAll(std::string re, std::string sample);

		// Для скомпилированного автомата поиск всех непересекающизся вхождений (без доступа к гр.з.)
		std::vector<std::string> FindAll(std::string sample);

		// ДКА в регулярку
		std::string KPath(); // !!!!!!!!!!!!!!
	};
}
*/