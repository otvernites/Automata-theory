#include "pch.h"
#include <Re2DFA.h>
#include <Re2DFA.cpp>
#include <Node.cpp>
#include <Lexer.cpp>

using namespace MyLib;

TEST(RootSetter, SyntaxTreeTest) {
	Lexer tree = Lexer();
	Node* root = new Node("a", TypeOfNode::a_node);
	tree.SetRoot(root);
	EXPECT_EQ(root, tree.GetRoot());
	EXPECT_EQ(root->GetType(), TypeOfNode::a_node);
}

TEST(Re2TreeExceptions, SyntaxTreeTest) {
	Lexer tree = Lexer();
	std::string str = "#j";
	ASSERT_THROW(tree.Re2Tree(str), std::exception); 
	str = "x{0,4}";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "w{3,2}";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "dkfk{1,3dj";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "d|+djjf";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "fj|(.d)";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "f(d))dh";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "f||v";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "(1:j|k)kk(1:qwerty)";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "fjf\\j";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "()";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
	str = "";
	ASSERT_THROW(tree.Re2Tree(str), std::exception);
}

TEST(FindAll, RegexTest) {

	DFA automat;

	DFA* dfa = automat.Compile("#||(a.c)+|#+");
	std::vector<std::string> substr = automat.FindAll("jcdhac|+hacacacj");
	std::vector<std::string> true_substr;
	true_substr.push_back("+");
	true_substr.push_back("ac");
	true_substr.push_back("acacac");
	true_substr.push_back("|");
	ASSERT_EQ(substr, true_substr);

	std::vector<std::string> substr1 = automat.FindAll("(1:s{2,3})(\\1)", "meowsssshssssss|sssss");
	std::vector<std::string> true_substr1;
	true_substr1.push_back("ssss");
	true_substr1.push_back("sssss");
	true_substr1.push_back("ssssss");
	ASSERT_EQ(substr1, true_substr1);

	substr1 = automat.FindAll("adj|^", "^");
	//std::vector<std::string> true_substr1;
	true_substr1.clear();
	true_substr1.push_back("");
	ASSERT_EQ(substr1, true_substr1);
}

TEST(KPath, RegexTest) {

	DFA* re = new DFA();
	DFA* re1 = new DFA();

	std::string reg = "((b|a)(b|a)(b|a)+)+";
	re->Compile(reg);
	int size1 = re->GetMinStates();
	std::string str = re->KPath();
	re1->Compile(str);
	int size2 = re1->GetMinStates();
	ASSERT_EQ(size1, size2);

	std::vector<std::string> res1 = re->FindAll("L(2:hd|h)(7:f)\\2(\\7L|D)+", "ccLhfhDcbcbdhfhfLsos");
	str = re->KPath();
	std::vector<std::string> res2 = re->FindAll(str, "ccLhfhDcbcbdhfhfLsos");
	ASSERT_EQ(res1, res2);

	reg = "^";
	re->Compile(reg);
	size1 = re->GetMinStates();
	str = re->KPath();
	re1->Compile(str);
	size2 = re1->GetMinStates();
	ASSERT_EQ(size1, size2);
}

TEST(Difference, RegexTest) {

	DFA* re1 = new DFA();
	DFA* re2 = new DFA();
	DFA* re3 = new DFA();
	DFA* result = new DFA();

	re1->Compile("a+");
	re2->Compile("b|a");
	ASSERT_NO_THROW(result->Difference(re1, re2));

	re3->Compile("a+");
	ASSERT_EQ(result->Difference(re1, re3), "");
}

TEST(Complement, RegexTest) {

	DFA* re1 = new DFA();
	DFA* result1 = new DFA();
	DFA* exp = new DFA();
	DFA* result = new DFA();

	re1->Compile("ba+");
	std::string str1 = result1->Complement(re1);
	auto re3 = exp->Complement(str1);
	result->Compile(re3);
	std::vector<std::string> set5 = re1->FindAll("kjdhbakjdkjbaaaajdhsb");
	std::vector<std::string> set6 = result->FindAll("kjdhbakjdkjbaaaajdhsb");

	re1->Compile("(1:b|c{1,2})\\1");
	str1 = result1->Complement(re1);
	re3 = exp->Complement(str1);
	result->Compile(re3);
	std::vector<std::string> set0 = re1->FindAll("bcbcccbbjbdudu");
	std::vector<std::string> set3 = result->FindAll("bcbcccbbjbdudu");
	
	re1->Compile("c.dt{3,}|dh{2}");
	str1 = result1->Complement(re1);
	re3 = exp->Complement(str1);
	result->Compile(re3);
	std::vector<std::string> set1 = re1->FindAll("djidjiwosjxttttdhdhdjscdt");
	std::vector<std::string> set2 = result->FindAll("djidjiwosjxttttdhdhdjscdt");

	re1->Compile("^");
	str1 = result1->Complement(re1);
	ASSERT_EQ("", str1);

	re1->Compile("L(2:hd|h)(7:f)\\2(\\7L|D)+");
	str1 = result1->Complement(re1);
	re3 = exp->Complement(str1);
	result->Compile(re3);
	std::vector<std::string> res1 = re1->FindAll("ccLhfhDcbcbdhfhfLsos");
	std::vector<std::string> res2 = result->FindAll("ccLhfhDcbcbdhfhfLsos");
	ASSERT_EQ(res1, res2);
}

TEST(CaptGroup, RegexTest) {

	DFA* re = new DFA();
	DFA* re1 = new DFA();
	DFA* result1 = new DFA();

	std::vector<std::string> full_res_1;
	full_res_1.push_back("gg");
	full_res_1.push_back("gg");
	full_res_1.push_back("ab");
	full_res_1.push_back("ab");

	std::vector<std::string> full_res_2;
	full_res_2.push_back("qbqb");
	full_res_2.push_back("qb");

	re->Compile("(1:(2:ab|g+)c)+d");
	std::vector<std::string> veco = re->FindAll("aabggcdcggcabcabcd");
	std::vector<std::string> gr1 = (*re)[2];
	ASSERT_EQ(full_res_1, gr1);


	re1->Compile("(3:a(1:t|tr)b(2:qb)+)");
	std::vector<std::string> veco1 = re1->FindAll("atrbqbqbatatbqbs");
	std::vector<std::string> gr2 = (*re1)[2];
	ASSERT_EQ(full_res_2, gr2);

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
