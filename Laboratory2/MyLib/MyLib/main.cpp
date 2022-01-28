#pragma once

#include "Re2DFA.h"

using namespace MyLib;

int main()
{
	DFA* re = new DFA();
	DFA* re1 = new DFA();
	//DFA* re2 = new DFA();
	DFA* result = new DFA();
	DFA* result1 = new DFA();

	//re->Compile("(a|bd+c)+bd+");
	re->Compile("a+");
	re1->Compile("ba+");
	
	//std::string str = result->Complement(re);
	std::string str1 = result1->Complement(re1);
	DFA* exp = new DFA();
	auto re3 = exp->Complement(str1);
	DFA* result3 = new DFA();
	result3->Compile(re3);

	//std::string res = result->Complement(re);
	//std::string res1 = result->Complement("^");
	//std::string reg = "#||(a.c)|#+";

	//re.Compile(reg);
	//std::string str = ".rotiiim+e";  
	//std::vector<std::string> vecan = re.FindAll(reg, str);
	//std::vector<std::string> veco = re.FindAll(str);

	//std::string regex = re.KPath();


	//std::string reg2 = "(1:i{2,3})(\\1)";
	//re.Compile(reg2);
	//std::string regex1 = re.KPath();
	//std::string reg1 = "(me)";
	//std::string stringa = "(so|do|ro)+(me)";
	//std::vector<std::string> vec = re.FindAll(reg2, str);
	//std::vector<std::string> vec1 = re.FindAll(regex1, str);
	std::cout << "hello world";
	// #|(1:djd)a{3,5}(2:jdj)d+(pb)+(676:evu+|b+)s\\676i\\1   // L(L|D)+ //L(2:hd|h)(7:f)\\2(\\7L|D)+
	
								//r(nr|#+)+(n|r)+n						// a{5}7{3,4}9(ab){1}		// L{2}(2:hd|h)4{3,}(7:f){1,2}\\2(\\7L|D)+																		        
	

										// (a|b)+abb
										// ((b|a)(b|a)(b|a)+)+ - 2 состояния объединить 
	

	return 0;
}