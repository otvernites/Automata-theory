#define _CRT_SECURE_NO_WARNINGS
#include "mainFunctions.h"

using namespace std;
using namespace statemap;

#include <iostream>
#include <windows.h>
#include <conio.h>

void (*dialogs[])(recognizerClass&) = {
        nullptr,
        Dialog_input,
        Dialog_input_file
};

int Get_solution_input() {
    const char* menu[] = {
        "0. Exit",
        "1. Keyboard input",
        "2. Input from file"
    };

    int solve = -1;
    do {
        system("cls");
        for (int i = 0; i < 3; i++) {
            std::cout << menu[i] << std::endl;
        }
        std::cout << "Input number of function" << std::endl;
        getNum(solve);
    } while ((solve < 0) || (solve > 2));

    return solve;
}


int main()
{
    recognizerClass aut;

    int solution = 0;
    while (solution = Get_solution_input()) {
        system("cls");
        dialogs[solution](aut);
        system("pause");
    }
}