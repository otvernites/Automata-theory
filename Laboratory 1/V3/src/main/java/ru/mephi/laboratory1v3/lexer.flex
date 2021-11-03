package ru.mephi.laboratory1v3;

import java.util.ArrayList;
import java.util.Objects;

%%

%class MyLexer
%public
%unicode
%line
%standalone

%{
    public class Pair {
        private String x = "";
        private int y = 0;

        public Pair(String x, int y)
        {
            this.x = x;
            this.y = y;
        }

        public String getX() {
            return x;
        }

        public void setX(String x) {
            this.x += x;
        }

        public int getY() {
            return y;
        }

        public void setY(int y) {
            this.y = y;
        }

        public void printPair() {
            System.out.println(this.x + "   - " + this.y);
        }
    }

    ArrayList<Pair> nameStat = new ArrayList<Pair>(); //for statistics
    ArrayList<Pair> results = new ArrayList<Pair>();  //for correct strings

    void appendSt(String fname) { //for statistics
        int key = 0;
        for (Pair p : nameStat) {
            if (Objects.equals(p.getX(), fname)) {
                p.setY(p.getY() + 1);
                key = 1;
                break;
            }
        }
        if (key == 0) {
            nameStat.add(new Pair(fname, 1));
        }
    }

    void appendResFirst(String str, int sentence, int res) { //for correct strings
        results.add(new Pair(Integer.toString(sentence) + " " + str, res));
    }

    void appendRes(String str, int sentence, int res) {
        int len = results.size()-1;
        if (Objects.equals(results.get(len).getX(), null)) {
            appendResFirst(str, sentence, res);
        } else {
            results.get(len).setX(str);
        }
        results.get(len).setY(res);
    }

    String name(String s) {
        int i = s.length();
        int j = i;
        while ((i > 0) && (s.charAt(i-1) != '/')) {
            --i;
        }

      return s.substring(i);
    }

%}

    Protocol = (nfs:)
    SName = [a-zA-Z]{1,15}
    Slash = \/
    Whitespace = [\ \t\b]
    LineTerminator = \r|\n|\r\n
    FName = ([a-zA-Z._]{1,20}\/([a-zA-Z._]{1,12}))|([a-zA-Z._]{1,12})

%state YYINITIAL
%state SNAME
%state FNAME
%state END

%%

    <YYINITIAL> {
        {Protocol}  {appendResFirst(yytext(), yyline, 0); yybegin(YYINITIAL);}
        {Slash}{Slash} {appendRes(yytext(), yyline, 0); yybegin(SNAME);}
        {Whitespace} {yybegin(YYINITIAL);}
        {LineTerminator} {yybegin(YYINITIAL);}
        <<EOF>> {return YYEOF;}
        (.) {yybegin(END);}
    }

    <SNAME>  {
        {SName}{Slash} {appendRes(yytext(), yyline, 0); yybegin(FNAME);}
        <<EOF>> {return YYEOF;}
        (.)|{LineTerminator} {results.remove(results.size()-1); yybegin(END);}
    }

    <FNAME> {
        {FName} {String s = yytext(); appendRes(s, yyline, 1); appendSt(name(s)); yybegin(YYINITIAL);}
        {LineTerminator} {yybegin(YYINITIAL);}
        <<EOF>> {return YYEOF;}
        (.) {results.remove(results.size()-1); yybegin(END);}
    }

    <END> {
        (.*){LineTerminator} {yybegin(YYINITIAL);}
        (.) {yybegin(END);}
    }