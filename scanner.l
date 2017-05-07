%{


#include <string>
using std::string;
#include <cstdlib>
using std::atoi;
#include <list>
using std::list;
#include <iostream>
using std::cout;    using std::endl;

#include <algorithm>
using std::transform;

#include "Node.h"
#include "parser.tab.h"
void yyerror(char *);
%}
%option yylineno
%%

[a-zA-Z][a-zA-Z0-9_]*    {
                            string temp = string(yytext);
                            if (temp == "return") {
                                return RETURN;
                            }
                            if (temp == "for") {
                                return FOR;
                            }
                            if (temp == "while") {
                                return WHILE;
                            }
                            if (temp == "if") {
                                return IF;
                            }
                            if (temp == "else") {
                                return ELSE;
                            }
                            if (temp == "break") {
                                return BREAK;
                            }
                            if (temp == "continue") {
                                return CONTINUE;
                            }
                            char buffer[80];
                            if (temp.size() > 12) {
                                sprintf(buffer, "Variable length exceeds 12: %s", yytext);
                                yyerror(buffer);
                                exit(0);
                            }
                            transform(temp.begin(), temp.end(), temp.begin(), tolower);
                            yylval.variable = new string(temp);
                            return VARIABLE;
                        }
0           {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[1-9][0-9]* {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[-()<>=+*/;{}.%\[\]@,] {
                return *yytext;
             }
"++"        return INC;
"--"        return DEC;
\"([^\"\\]|\\[ntr0\\\"])*\"  {
                yylval.sValue = new string(yytext);
                return STRING;
            }
\'(\\[ntr0\'\\]|[^\'\\])\'       {yylval.cValue = new string(yytext);  return CHARACTER;}
">="        return GE;
"<="        return LE;
"=="        return EQ;
"!="        return NE;
"&&"        return AND;
"||"        return OR;
\/\/.*\n    ;    /* skip comments */
"def "      return DEF;
[ \t\n]+    ;       /* ignore whitespace */

.           yyerror(yytext);
<<EOF>>     return END;
%%

int yywrap(void) {
    return 1;
}

void yyerror(char *s) {
    fprintf(stderr, "%d: %s at %s\n", yylineno, s, yytext);
    exit(-1);
}