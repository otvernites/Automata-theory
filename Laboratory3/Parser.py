#! /usr/bin/env python

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
        self.is_correct = True    # for creating tree

    def parse(self, s):
        try:
            result = self.parser.parse(s)
            return result, self.functions, self.is_correct
        except LexError:
            self.is_correct = False
            sys.stderr.write(f'Illegal token {s}\n')


    # Definition of rules
    def p_program(self, p):
        """program : statement_list NL
                   | statement_list
        """

    def p_statement_list(self, p):
        """statement_list : statement_list statement
                          | statement
                          | NL
        """

    def p_statement(self, p):
        """statement : declaration NL
                     | assignment NL
                     | arithmetic NL
                     | logics NL
                     | comparison NL
                     | cycle NL
                     | conditionals NL
                     | pass NL
                     | identification NL
                     | robot NL
        """

    def p_statement_error(self, p):
        '''statement : error'''
        self.is_correct = False
        sys.stderr.write(f'Syntax error: "{p[1].value}" at {p[1].lineno}:{p[1].lexpos}\n')

    # regular variables
    def p_variable(self, p):
        """variable : INT_VAR
                    | BOOL_VAR
                    | int_arr_var
                    | bool_arr_var
        """

    def p_procedure(self, p):
        """procedure : PROC_VAR
                    | proc_arr_var
        """

    def p_label(self, p):
        """label : LABEL"""

    def p_literal(self, p):
        """literal : INT
                | TRUE
                | FALSE
        """

    def p_var_call_id(self, p):  # unsigned value
        """var_call_id : VAR_CALL"""

    # arrays
    def p_int_arr_var(self, p):
        """int_arr_var : INT_ARR id_list"""

    def p_bool_arr_var(self, p):
        """bool_arr_var : BOOL_ARR id_list"""

    def p_proc_arr_var(self, p):
        """proc_arr_var : PROC_ARR id_list"""

    def p_id_list(self, p):
        """id_list : id_list COMMA id
                   | id
        """

    def p_id(self, p):
        """id : var_call_id
              | INT_VAR
              | arr_call_id
        """

    def p_arr_call_id(self, p): # array element access
        """arr_call_id : ARR_CALL id_list"""

    def p_call(self, p): # common call (getters)
        """call : arr_call_id
                | var_call_id
        """

    # statements

    # declaration
    def p_declaration(self, p):
        """declaration : label
                       | declaration_set
        """

    def p_declaration_set(self, p):
        """declaration_set : variable
                           | procedure
        """

    # assignment
    # здесь нужно много условий на проверку, что вызывается именно процедура!
    def p_assignment(self, p):
        """assignment : declaration_set ASSIGN call
                      | declaration_set ASSIGN declaration_set
                      | declaration_set ASSIGN literal
                      | var_call_id ASSIGN call
                      | var_call_id ASSIGN LBRACE statement_list RBRACE
                      | var_call_id ASSIGN variable
                      | var_call_id ASSIGN literal
                      | BOOL_VAR ASSIGN robot
                      | call ASSIGN robot
                      | variable ASSIGN variable
                      | variable ASSIGN literal
        """

    # math operations
    def p_arithmetic(self, p):
        """arithmetic : INCR
                      | DECR
        """

    # logical operations
    # тут надо проверять, что call обращается к bool
    def p_log_type(self, p):
        """log_type : TRUE
                    | FALSE
                    | BOOL_VAR
                    | call
        """

    def p_logic_expr(self, p):
        """logic_expr : logic_expr COMMA log_type
                      | log_type
        """

    def p_logics(self, p):
        """logics : NOR logic_expr
        """

    # comparison operations
    # проверить, что в call вызов процедуры
    def p_comparison(self, p):
        """comparison : arithmetic EQ INT
                      | logics EQ TRUE
                      | logics EQ FALSE
                      | logic_expr EQ TRUE
                      | logic_expr EQ FALSE
                      | call EQ PASS
        """

    # сycle operations
    def p_cycle(self, p):
        """cycle : LPAREN logics RPAREN LBRACE statement_list RBRACE
                 | LPAREN logic_expr RPAREN LBRACE statement_list RBRACE
        """

    # conditional transitions
    # отслеживать что var_call_id вызывает именно метку
    def p_conditionals(self, p):
        """conditionals : LBRACKET logics RBRACKET GOTO var_call_id
                        | LBRACKET logic_expr RBRACKET GOTO var_call_id
                        | LBRACKET logics RBRACKET var_call_id
                        | LBRACKET logic_expr RBRACKET var_call_id
                        | GOTO var_call_id
                        | var_call_id
        """

    # pass
    def p_pass(self, p):
        """pass : PASS"""

    # identification operations
    # проверить, что в call вызов процедуры
    def p_identification(self, p):
        """identification : variable LINK var_call_id
                          | procedure LINK var_call_id
                          | variable LINK_BREAK var_call_id
                          | procedure LINK_BREAK var_call_id
        """

    # robot operations
    def p_robot(self, p):
        """robot : FORWARD
                 | BACK
                 | RIGHT
                 | LEFT
                 | TP
        """

    def p_error(self, p):
        try:
            sys.stderr.write(f'Syntax error at {p.lineno} line\n')
        except Exception:
            sys.stderr.write(f'Syntax error\n')
        self.is_correct = False


if __name__ == '__main__':
    parser = Parser()
    f = open('test1.txt', 'r')
    data = f.read()
    f.close()

    tree = parser.parser.parse(data, debug=True)
