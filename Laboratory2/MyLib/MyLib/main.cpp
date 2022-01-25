#pragma once

#include "Re2DFA.h"

using namespace MyLib;

int main()
{
	DFA re = DFA();
	std::string str = "^(f|c)^ w .	^c";     // #|(1:djd)a{3,5}(2:jdj)d+(pb)+(676:evu+|b+)s\\676i\\1   // L(L|D)+ //L(2:hd|h)(7:f)\\2(\\7L|D)+
	
								//r(nr|#+)+(n|r)+n						// a{5}7{3,4}9(ab){1}		// L{2}(2:hd|h)4{3,}(7:f){1,2}\\2(\\7L|D)+																		        
	

	re.Compile(str);									// (a|b)+abb
										// ((b|a)(b|a)(b|a)+)+ - 2 состояния объединить 
	

	return 0;
}