#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "recognizerClass.h"
#include <chrono>

template <class T>
double getNum(T& a) {
	std::cin >> a;
	while (!std::cin.good()) {
		std::cin.clear();
		std::cout << "Invalid input, try again: \n";
		std::cin.ignore(INT_MAX, '\n');
		std::cin.clear();
		std::cin >> a;
	}
	return a;
}

// output
void Dialog_output(recognizerClass& aut, const std::vector<pair<string, bool>> & vec) {
	std::vector<pair<string, bool>>::const_iterator it;
	for (it = vec.begin(); it != vec.end(); ++it) {
		cout << it->first << " - - - " << std::boolalpha << it->second << endl;
	}
	
	std::map<std::string, int> st = aut.getStat();

	cout << "\n -----Statistics-----" << endl;
	for (auto it = st.begin(); it != st.end(); ++it) {
		cout << (*it).first << " - " << (*it).second << endl;
	}
}

void Dialog_output_file(recognizerClass& aut, const std::vector<pair<string, bool>>& vec) {
	ofstream out;
	out.open("Result2.txt");
	if (!out.is_open()) {
		cerr << "Error writing to file\n";
		return;
	}
	std::vector<pair<string, bool>>::const_iterator it;
	for (it = vec.begin(); it != vec.end(); ++it) {
		out << it->first << " - - - " << std::boolalpha << it ->second << endl;
	}

	std::map<std::string, int> st = aut.getStat();

	out << "\n -----Statistics-----" << endl;
	for (auto it = st.begin(); it != st.end(); ++it) {
		out << (*it).first << " - " << (*it).second << endl;
	}
	out.close();
}

void (*print[])(recognizerClass& term, const std::vector<pair<string, bool>>& vec) = {
		Dialog_output,
		Dialog_output_file
};

int Get_solution_output() {
	const char* menu[] = {
		"0. Console output",
		"1. Output to file"
	};

	int solve = -1;
	do {
		system("cls");
		for (int i = 0; i < 2; i++) {
			std::cout << menu[i] << std::endl;
		}
		std::cout << "Input number of function" << std::endl;
		getNum(solve);
	} while ((solve < 0) || (solve > 1));

	return solve;
}

// input
void Dialog_input(recognizerClass& aut) {
	cout << "Enter the number of lines" << endl;
	int a;
	getNum(a);
	std::vector<pair<string,bool>> vec;
	for (int i = 0; i < a; ++i) {
		cout << "Enter your string:" << endl;
		string str;
		cin >> str;
		vec.push_back(pair<string, bool>(str,false));
		cin.clear();
		cin.ignore(INT_MAX, '\n');
	}

	for (int i = 0; i < vec.size(); ++i) { // processing each string
		char* newMas = nullptr;
		int len = vec[i].first.length();
		newMas = new char[len + 1];
		for (int j = 0; j < len; ++j) {
			newMas[j] = vec[i].first[j];
		}
		newMas[len] = '\0';
		vec[i].second = aut.CheckString(newMas); 
		delete[] newMas;
	}

	int solution = Get_solution_output();
	print[solution](aut, vec);
}

void Dialog_input_file(recognizerClass& aut) {
	cout << "Enter the name of the file to open:" << endl;
	string infoFile;
	cin >> infoFile;
	ifstream f(infoFile);
	if (!f) {
		cerr << "Cannot open the file\n";
		return;
	}
	infoFile.clear();

	std::vector<pair<string, bool>> vec;
	while (getline(f, infoFile)) {   
		vec.push_back(pair<string, bool>(infoFile, false));
	}
	f.close();

	std::chrono::nanoseconds time(0);

	for (int i = 0; i < vec.size(); ++i) { // processing each string
		char* newMas = nullptr;
		int len = vec[i].first.length();
		newMas = new char[len + 1];
		for (int j = 0; j < len; ++j) {
			newMas[j] = vec[i].first[j];
		}
		newMas[len] = '\0';

		auto startTime = std::chrono::high_resolution_clock::now();
		vec[i].second = aut.CheckString(newMas); 
		auto endTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
		time += duration;

		delete[] newMas;
		
	}

	cout << time.count() << endl;
	cin.ignore();
	getchar();
	int solution = Get_solution_output();
	print[solution](aut, vec);
}




