#! /usr/bin/env python

import ply.lex as lex
import re
import sys

reserved = {
    ',#': 'INCR', ',*': 'DECR', '<-': 'ASSIGN', '.#': 'NOR', 'eq': 'EQ', 'please': 'GOTO',
    'np': 'PASS', '@': 'LINK', '%': 'LINK_BREAK', 'mf': 'FORWARD', 'mb': 'BACK',
    'mr': 'RIGHT', 'ml': 'LEFT', 'tp': 'TP'
}


class Lexer:

    def __init__(self):
        self.lexer = lex.lex(module=self)

    def input(self, data):
        return self.lexer.input(data)

    def token(self):
        return self.lexer.token()

    tokens = list(reserved.values()) + [
        # variables and auxiliary elements
        'INT_VAR', 'BOOL_VAR', 'PROC_VAR',
        'INT_ARR', 'BOOL_ARR', 'PROC_ARR',
        'LABEL', 'ARR_CALL', 'VAR_CALL',

        # literals
        'INT',
        'TRUE', 'FALSE',

        # delimeters ( ) [ ] { } , . $ ~ : \n
        'LPAREN', 'RPAREN',
        'LBRACKET', 'RBRACKET',
        'LBRACE', 'RBRACE',
        'COMMA', 'NL',
    ]

    t_LPAREN= r'\('
    t_RPAREN = r'\)'
    t_LBRACKET = r'\['
    t_RBRACKET = r'\]'
    t_LBRACE = r'\{'
    t_RBRACE = r'\}'
    t_COMMA = r'\,'
    t_ignore = ' \t'

    def t_ARR_CALL(self, t):
        r'\d+\:(?!\w)'
        t.value = int(t.value[:-1])
        return t

    def t_INT(self, t):
        r'[\+-]?\d+\,(?!\w)'
        t.value = int(t.value[:-1])
        return t

    def t_VAR_CALL(self, t):
        r'\d+(?!\w)'
        t.value = int(t.value)
        return t

    def t_TRUE(self, t):
        r'T(?!\w)'
        t.value = True
        return t

    def t_FALSE(self, t):
        r'F(?!\w)'
        t.value = False
        return t

    def t_LABEL(self, t):
        r'\~\d+(?!\w)'
        t.value = int(t.value[1:])
        return t

    def t_INT_ARR(self, t):
        r'\,\d+\:(?!\w)'
        t.value = int(t.value[1 : -1])
        return t

    def t_BOOL_ARR(self, t):
        r'\.\d+\:(?!\w)'
        t.value = int(t.value[1 : -1])
        return t

    def t_PROC_ARR(self, t):
        r'\$\d+\:(?!\w)'
        t.value = int(t.value[1 : -1])
        return t

    def t_INT_VAR(self, t):
        r'\,\d+(?!\w)'
        t.value = int(t.value[1:])
        return t

    def t_BOOL_VAR(self, t):
        r'\.\d+(?!\w)'
        t.value = int(t.value[1:])
        return t

    def t_PROC_VAR(self, t):
        r'\$\d+(?!\w)'
        t.value = int(t.value[1:])
        return t

    def t_INCR(self, t):
        r'\,\#\d+(?!\w)'
        t.value = int(t.value[2:])
        return t

    def t_DECR(self, t):
        r'\,\*\d+(?!\w)'
        t.value = int(t.value[2:])
        return t

    def t_ASSIGN(self, t):
        r'\<\-'
        return t

    def t_NOR(self, t):
        r'\.\#'
        return t

    def t_EQ(self, t):
        r'eq'
        return t

    def t_GOTO(self, t):
        r'please'
        return t

    def t_PASS(self, t): #
        r'np(?!\w)'
        return t

    def t_LINK(self, t):
        r'\@(?!\w)'
        return t

    def t_LINK_BREAK(self, t):
        r'\%(?!\w)'
        return t

    def t_FORWARD(self, t):
        r'mf(?!\w)'
        return t

    def t_BACK(self, t):
        r'mb(?!\w)'
        return t

    def t_RIGHT(self, t):
        r'mr(?!\w)'
        return t

    def t_LEFT(self, t):
        r'ml(?!\w)'
        return t

    def t_TP(self, t):
        r'tp(?!\w)'
        return t

    def t_NL(self, t):
        r'\n+'
        t.lexer.lineno += t.value.count('\n')
        return t

    def t_error(self, t):
        sys.stderr.write(f'Illegal character: "{t.value}" at line {t.lexer.lineno}\n')
        t.lexer.skip(len(t.value))
        t.lexer.begin('INITIAL')


if __name__ == '__main__':
    lexer = Lexer()
    f = open('test2.txt', 'r')
    data = f.read()
    f.close()

    try:
        lexer.input(data)
        while True:
            tok = lexer.token()
            if tok is None:
                break
            else:
                print(tok)
    except EOFError:
        pass