#! /usr/bin/env python
import sys
import ply.yacc as yacc
from ply.lex import LexError

from Variable import Array, Variable
from Lexer.Lexer import Lexer
from Parser.SyntaxTree import Node


class Parser:

    tokens = Lexer.tokens

    def __init__(self):
        self.lexer = Lexer()
        self.parser = yacc.yacc(module=self, debug=False)
        self.declaration = dict()  # уникальный номер - что делает
        self.error = False

    def parse(self, s):
        try:
            result = self.parser.parse(s)
            return result, self.declaration, self.error
        except LexError:
            self.error = True
            sys.stderr.write(f'Illegal token {s}\n')

    # Definition of rules
    def p_program(self, p):
        """program : statement_block NL
                   | statement_block
        """
        p[0] = Node('program', children=[p[1]], lineno=p.lineno(1))

    def p_program_error(self, p):
        """program : error NL
                   | error
                   | statement_block error
                   | statement_block error NL
        """
        self.error = True
        sys.stderr.write(f'Program error at {p[1].lineno}\n')
        p[0] = Node('error', value="program error", children=[p[1]], lineno=p.lineno(1))

    def p_statement_block(self, p):
        """statement_block : statement_block statement
                           | statement
        """
        if len(p) == 2:
            p[0] = Node('statement', children=[p[1]], lineno=p.lineno(1))
        else:
            p[0] = Node('statement_block', children=[p[1],p[2]], lineno=p.lineno(1))

    def p_statement_block_error(self, p):
        """statement_block : error
        """
        self.error = True
        sys.stderr.write(f'Statement block error: "{p[1].value}" at {p[1].lineno}:{p[1].lexpos}\n')
        p[0] = Node('error', value="statement block error", children=[p[1]], lineno=p.lineno(1))

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
                     | empty
                     | LPAREN statement RPAREN NL
        """
        if len(p) == 5:
            p[0] = p[2]
        else:
            p[0] = p[1]

    def p_statement_error(self, p):
        '''statement : error
                     | LPAREN error RPAREN NL
        '''
        self.error = True

        if len(p) != 5:
            p[0] = Node('error', value="syntax error", children=[p[1]], lineno=p.lineno(1))
            sys.stderr.write(f'Syntax error: "{p[1].value}" at {p[1].lineno}:{p[1].lexpos}\n')
        else:
            p[0] = Node('error', value="syntax error", children=[p[2]], lineno=p.lineno(1))
            sys.stderr.write(f'Syntax error: "{p[2].value}" at {p[2].lineno}:{p[2].lexpos}\n')

    # regular variables
    def p_variable_int(self, p):
        """variable : INT_VAR"""
        p[0] = Node('int_var', value=p[1], lineno=p.lineno(1))

    def p_variable_bool(self, p):
        """variable : BOOL_VAR"""
        p[0] = Node('bool_var', value=p[1], lineno=p.lineno(1))

    def p_variable(self, p):
        """variable : int_arr_var
                    | bool_arr_var
        """
        p[0] = p[1]

    def p_procedure_simple(self, p):
        """procedure : PROC_VAR"""
        p[0] = Node('procedure', value=p[1], lineno=p.lineno(1))
        var = Variable("proc", int(p[1]))
        self.declaration[var.name] = var

    def p_procedure(self, p):
        """procedure : proc_arr_var"""
        p[0] = p[1]
        str_view = p[1].__str__()
        id1 = str_view.find(':')
        id2 = str_view.find('(')

        var = Array("proc_arr", int(str_view[id1+2:id2-1]))
        self.declaration[var.name] = var

    def p_label(self, p):
        """label : LABEL"""
        p[0] = Node('label', value=p[1], lineno=p.lineno(1))
        var = Variable("label", int(p[1]))
        self.declaration[var.name] = var

    def p_literal(self, p):
        """literal : LPAREN INT RPAREN
                   | INT
                   | TRUE
                   | FALSE
        """
        if len(p) == 4:
            p[0] = Node('literal', value=p[2], lineno=p.lineno(1))
        else:
            p[0] = Node('literal', value=p[1], lineno=p.lineno(1))

    def p_var_call_id(self, p):  # unsigned value
        """var_call_id : VAR_CALL"""
        p[0] = p[1]
        p[0] = Node('var_call', value=p[1], lineno=p.lineno(1))

    def p_arr_call_id(self, p): # array element access
        """arr_call_id : ARR_CALL LBRACKET id_list RBRACKET"""
        p[0] = Node('arr_call', value=p[1], children=[p[3]], lineno=p.lineno(1))

    def p_call(self, p): # common call (getters)
        """call : arr_call_id
                | var_call_id
        """
        p[0] = p[1]

    # arrays
    def p_int_arr_var(self, p):
        """int_arr_var : INT_ARR LBRACKET id_list RBRACKET"""
        p[0] = Node('int_arr_var', value=p[1], children=[p[3]], lineno=p.lineno(1))

    def p_bool_arr_var(self, p):
        """bool_arr_var : BOOL_ARR LBRACKET id_list RBRACKET"""
        p[0] = Node('bool_arr_var', value=p[1], children=[p[3]], lineno=p.lineno(1))

    def p_proc_arr_var(self, p):
        """proc_arr_var : PROC_ARR LBRACKET id_list RBRACKET"""
        p[0] = Node('proc_arr_var', value=p[1], children=[p[3]], lineno=p.lineno(1))

    def p_id_list(self, p):
        """id_list : id_list id
                   | id
        """
        if len(p) == 3:
            p[0] = Node('id_list', children=[p[1], p[2]], lineno=p.lineno(1))
        else:
            p[0] = Node('id_list', children=[p[1]], lineno=p.lineno(1))

    # проверять, что индексы - целые или по индексам целые числа (и они больше нуля)
    def p_id_simple(self, p):
        """id : INT"""
        p[0] = Node('id', value=p[1], lineno=p.lineno(1))

    def p_id(self, p):
        """id : call"""
        p[0] = p[1]

    # ~~~ statements ~~~ #
    # declaration
    def p_declaration(self, p):
        """declaration : label
                       | declaration_set
        """
        p[0] = Node('declaration', children=[p[1]], lineno=p.lineno(1))

    def p_declaration_error(self, p):
        '''declaration : error'''
        self.error = True
        sys.stderr.write(f'Invalid declaration: "{p[1].value}" at {p[1].lineno}:{p[1].lexpos}\n')
        p[0] = Node('error', value="declaration error", children=[p[1]], lineno=p.lineno(1))

    def p_declaration_set(self, p):
        """declaration_set : variable
                           | procedure
        """
        p[0] = p[1]

    # assignment
    # здесь нужно много условий на проверку, что вызывается именно процедура!
    # т.е. в последних двух насчет процедур (+работает ли вообще предпоследнее?) (там еще call ass call)
    def p_assignment(self, p):
        """assignment : declaration ASSIGN literal
                      | declaration ASSIGN call

                      | call ASSIGN call
                      | call ASSIGN literal

                      | declaration ASSIGN arithmetic
                      | declaration ASSIGN logics
                      | declaration ASSIGN comparison
                      | declaration ASSIGN identification
                      | declaration ASSIGN robot

                      | call ASSIGN arithmetic
                      | call ASSIGN logics
                      | call ASSIGN comparison
                      | call ASSIGN identification
                      | call ASSIGN robot

                      | call ASSIGN LBRACE NL statement_block RBRACE
        """
        if len(p) == 4:
            p[0] = Node('assignment', value=p[2], children=[p[1], p[3]], lineno=p.lineno(1))
        else:
            p[0] = Node('assignment', children=[p[1], p[5]], lineno=p.lineno(1))

    def p_assignment_error(self, p):
        """assignment : error ASSIGN call
                      | error ASSIGN literal
                      | declaration ASSIGN error

                      | call ASSIGN error

                      | error ASSIGN arithmetic
                      | error ASSIGN logics
                      | error ASSIGN comparison
                      | error ASSIGN identification
                      | error ASSIGN robot

                      | error ASSIGN LBRACE statement_block RBRACE
                      | call ASSIGN LBRACE error RBRACE
        """
        self.error = True
        if len(p) == 6:
            p[0] = Node('error', value="assignment error", children=[p[1],p[4]], lineno=p.lineno(1))
            sys.stderr.write(f'Invalid assignment at {p[0].lineno}\n')
        else:
            p[0] = Node('error', value="assignment error", children=[p[1], p[3]], lineno=p.lineno(1))
            sys.stderr.write(f'Invalid assignment at {p[0].lineno}\n')

    # math operations
    def p_arithmetic(self, p):
        """arithmetic : INCR
                      | DECR
        """
        if p[1][:2] == ",#":
            p[0] = Node('incr', value=p[1][2:], lineno=p.lineno(1))
        elif p[1][:2] == ",*":
            p[0] = Node('decr', value=p[1][2:], lineno=p.lineno(1))

    def p_arithmetic_error(self, p):
        '''arithmetic : error'''
        self.error = True
        sys.stderr.write(f'Invalid arithmetic operation: "{p[1].value}" at {p[1].lineno}:{p[1].lexpos}\n')
        p[0] = Node('error', value="math error", children=[p[1]], lineno=p.lineno(1))

    # logical operations
    # тут надо проверять, что call обращается к bool
    def p_log_type(self, p):
        """log_type : TRUE
                    | FALSE
                    | call
        """

        if (p[1] is not True) and (p[1] is not False):
            p[0] = p[1]
        else:
            p[0] = Node('log_type', value=p[1], lineno=p.lineno(1))

    def p_logic_expr(self, p):
        """logic_expr : logic_expr COLON log_type
                      | log_type
        """
        if len(p) == 4:
            p[0] = Node('logic_expr', children=[p[1], p[3]], lineno=p.lineno(1))
        else:
            p[0] = p[1]

    def p_logics(self, p):
        """logics : NOR logic_expr
        """
        p[0] = Node('logics', children=[p[2]], lineno=p.lineno(1))

    def p_logics_error(self, p):
        '''logics : NOR error'''
        self.error = True
        sys.stderr.write(f'Invalid logical operation: "{p[2].value}" at {p[2].lineno}:{p[2].lexpos}\n')
        p[0] = Node('error', value="logics error", children=[p[2]], lineno=p.lineno(1))

    # comparison operations
    # проверить, что в call вызов процедуры
    def p_comparison(self, p):
        """comparison : arithmetic EQ INT
                      | logics EQ TRUE
                      | logics EQ FALSE
                      | logic_expr EQ TRUE
                      | logic_expr EQ FALSE
                      | call EQ PASS
                      | call EQ literal
        """
        p[0] = Node('comparison', value=p[2], children=[p[1], p[3]], lineno=p.lineno(1))

    def p_comparison_error(self, p):
        """comparison : error EQ INT
                      | error EQ TRUE
                      | error EQ FALSE
                      | error EQ PASS

                      | arithmetic EQ error
                      | logics EQ error
                      | logic_expr EQ error
                      | call EQ error

                      | arithmetic EQ error conditionals
                      | logics EQ error conditionals
        """
        self.error = True
        sys.stderr.write(f'Invalid comparison operation at {p[1].lineno}\n')
        p[0] = Node('error', value="comparison error", children=[p[1], p[3]], lineno=p.lineno(1))

    # сycle operations
    def p_cycle(self, p):
        """cycle : LPAREN logics RPAREN LBRACE NL statement_block RBRACE
                 | LPAREN logic_expr RPAREN LBRACE NL statement_block RBRACE
        """
        p[0] = Node('cycle', children=[p[2],p[6]], lineno=p.lineno(1))

    def p_cycle_error(self, p):
        """cycle : LPAREN error RPAREN LBRACE NL statement_block RBRACE
                 | LPAREN logics RPAREN LBRACE NL error RBRACE
                 | LPAREN logic_expr RPAREN LBRACE NL error RBRACE
        """
        self.error = True
        sys.stderr.write(f'Invalid cycle at {p[2].lineno}\n')
        p[0] = Node('error', value="cycle error", children=[p[2],p[6]], lineno=p.lineno(1))

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
        if len(p) == 6:
            p[0] = Node('conditionals', children=[p[2],p[5]], lineno=p.lineno(1))
        elif len(p) == 5:
            p[0] = Node('conditionals', children=[p[2],p[4]], lineno=p.lineno(1))
        elif len(p) == 3:
            p[0] = Node('conditionals', children=[p[2]], lineno=p.lineno(1))
        elif len(p) == 2:
            p[0] = Node('conditionals', children=[p[1]], lineno=p.lineno(1))

    def p_conditionals_error(self, p):
        """conditionals : LBRACKET error RBRACKET GOTO var_call_id
                        | LBRACKET error RBRACKET var_call_id

                        | LBRACKET logics RBRACKET GOTO error
                        | LBRACKET logic_expr RBRACKET GOTO error
                        | LBRACKET logics RBRACKET error
                        | LBRACKET logic_expr RBRACKET error

                        | GOTO error
                        | error
        """
        self.error = True
        sys.stderr.write(f'Invalid condition at {p[2].lineno}\n')
        if len(p) == 6:
            p[0] = Node('error', value="conditional error", children=[p[2],p[5]], lineno=p.lineno(1))
        elif len(p) == 5:
            p[0] = Node('error', value="conditional error", children=[p[2],p[4]], lineno=p.lineno(1))
        elif len(p) == 3:
            p[0] = Node('error', value="conditional error", children=[p[2]], lineno=p.lineno(1))
        elif len(p) == 2:
            p[0] = Node('error', value="conditional error", children=[p[1]], lineno=p.lineno(1))

    # pass
    def p_pass(self, p):
        """pass : PASS"""
        p[0] = Node('pass', lineno=p.lineno(1))

    # identification operations
    # проверить, что в call вызов процедуры
    def p_identification(self, p):
        """identification : declaration LINK var_call_id
                          | call LINK var_call_id
                          | declaration LINK_BREAK var_call_id
                          | call LINK_BREAK var_call_id
        """
        p[0] = Node('identification', value=p[2], children=[p[1], p[3]], lineno=p.lineno(1))

    def p_identification_error(self, p):
        """identification : error LINK var_call_id
                          | error LINK_BREAK var_call_id

                          | declaration LINK error
                          | call LINK error
                          | declaration LINK_BREAK error
                          | call LINK_BREAK error

                          | identification error
                          | literal LINK var_call_id
                          | literal LINK_BREAK var_call_id
        """
        self.error = True
        if len(p) == 4:
            sys.stderr.write(f'Invalid identification at {p[1].lineno}\n')
            p[0] = Node('error',  value="identification error", children=[p[1], p[3]], lineno=p.lineno(1))
        else:
            sys.stderr.write(f'Invalid identification at {p[2].lineno}\n')
            p[0] = Node('error',  value="identification error",  lineno=p.lineno(1))

    # robot operations
    def p_robot(self, p):
        """robot : FORWARD
                 | BACK
                 | RIGHT
                 | LEFT
                 | TP
        """
        p[0] = Node('robot', value=p[1], lineno=p.lineno(1))

    def p_empty(self, p):
        """empty : NL"""
        p[0] = Node('empty', value=p[1], lineno=p.lineno(1))

    def p_error(self, p):
        self.error = True
        try:
            sys.stderr.write(f'> syntax error at {p.lineno} line\n')
        except Exception:
            sys.stderr.write(f'> syntax error\n')


if __name__ == '__main__':
    parser = Parser()
    f = open('Tests/test_parser_program.txt', 'r')
    data = f.read()
    f.close()

    result = parser.parser.parse(data, debug=True)
    try:
        dot = result.print_tree()
        dot.write_png('Parser/tree.png')
    except Exception:
        sys.stderr.write(f'> Tree building error! Correct your program.\n')


