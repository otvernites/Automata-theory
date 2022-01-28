/*
#pragma once
#include "Re2DFA.h"

namespace MyLib {
	// �������
	class ReLib : public DFA {
	private:
		std::map<std::tuple<int, int, int>, std::string> subpaths; // k-���� ��� ���� ���������, �� ������� �������� ��� (������, ����, ������������� ���������) !!!!!!!!!!!

	protected:
		// ----------
		void KPathInduction(int k, int id_from, int id_to, std::map<std::pair<int, int>, std::vector<std::string>> paths); // !!!!!!!!!!

	public:
		ReLib() : DFA() {}

		// ��� ����������� ��������� ����� ���� ���������������� ��������� (��� ������� � ��.�.)
		std::vector<std::string> FindAll(std::string re, std::string sample);

		// ��� ����������������� �������� ����� ���� ���������������� ��������� (��� ������� � ��.�.)
		std::vector<std::string> FindAll(std::string sample);

		// ��� � ���������
		std::string KPath(); // !!!!!!!!!!!!!!
	};
}
*/