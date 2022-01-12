#pragma once

#include "Re2DFA.h"

using namespace MyLib;

int main()
{
	DFA re = DFA();
	std::string str = "L(2:hd|h)(7:f)\\2(\\7L|D)+";     // #|(1:djd)a{3,5}(2:jdj)d+(pb)+(676:evu+|b+)s\\676i\\1   // L(L|D)+
	re.Re2Tree(str);

	Node* root = re.GetRoot();
	re.Re2DFA(root);
	re.CreateTreeImg();
	//re.CreateDFAImg();

	return 0;
}