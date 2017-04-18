%{
#include <cstdlib>
using std::atoi;
#include <cstring>
using std::strlen;
#include <string>
using std::string;  using std::tolower;
#include <algorithm>
using std::transform;
#include "c6.h"
#include "c6.tab.h"
void yyerror(char *);
%}

%%

[a-zA-Z][a-zA-Z0-9]*    {
                            string temp = string(yytext);
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
                            transform(temp.begin(), temp.end(), temp.begin(), [](char i) -> char {return tolower(i);});
                            yylval.variable = strdup(yytext);
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

[-()<>=+*/;{}.%\[\]@] {
                return *yytext;
             }
\"[^\"]*\"  {
                yylval.sValue = strdup(yytext);
                return STRING;
            }
\'[^\']\'   {yylval.cValue = yytext[1]; return CHARACTER;}
">="        return GE;
"<="        return LE;
"=="        return EQ;
"!="        return NE;
"&&"        return AND;
"||"        return OR;
"def "      return DEF;
\/\/.*\n    ;    /* skip comments */

[ \t\n]+    ;       /* ignore whitespace */

.           yyerror("Unknown character");

%%

int yywrap(void) {
    return 1;
}
