#! /usr/bin/env python

# from __future__ import annotations
# #from typing import List, Dict, Tuple
import sys
import ply.yacc as yacc
from Lexer import Lexer
from ply.lex import LexError


class Parser:

    tokens = Lexer.tokens

    def __init__(self):
        self.lexer = Lexer()
        self.parser = yacc.yacc(module=self, debug=False)
        self.functions = dict()
        self.is_correct = True    # creating tree

    def parse(self, s):
        try:
            result = self.parser.parse(s)
            return result, self.functions, self.is_correct
        except LexError:
            self.is_correct = False
            sys.stderr.write(f'Illegal token {s}\n')


    # Definition of rules
    def p_program(self, p):
        """program : statement_list NL"""

    def p_statement_list(self, p):
        """statement_list : statement_list statement NL
                          | statement NL
        """

    def p_statement(self, p):
        """statement : assignment NL
                     | logics NL
                     | comparison NL
                     | cycle NL
                     | conditionals NL
                     | linking NL
                     | breaking NL
                     | robot NL
        """

    def p_variable(self, p):
        """variable : LABEL
                    | VAR_CALL
                    | INT_VAR
                    | BOOL_VAR
                    | PROC_VAR
                    | INT_ARR
                    | BOOL_ARR
                    | PROC_ARR
        """

    def p_literal(self, p):
        """literal : INT
                   | TRUE
                   | FALSE
        """

    # Assignment operation
    def p_assignment(self, p):
        """assignment :
         """


if __name__ == '__main__':
    parser = Parser()
    f = open('test1.txt', 'r')
    data = f.read()
    f.close()

    try:
        tree, functions, is_correct = parser.parse(data)
        while True:
            for key, value in parser.functions.items():
                print(key)
                value.print()
    except EOFError:
        pass




    """
    if tree is not None and ok is True:
        tree.print()
        print(functions)
        functions['main'].print()
    else:
        print('error tree built')
    """

