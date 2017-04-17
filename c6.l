%{
#include <cstdlib>
using std::atoi;
#include <cstring>
using std::strlen;
#include <iostream>
using std::cout;    using std::endl;
#include "c6.h"
#include "c6.tab.h"
void yyerror(char *);
%}

%%

[a-zA-Z][a-zA-Z0-9_]*   {yylval.variable = strdup(yytext); return VARIABLE;}
0           {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[1-9][0-9]* {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[-()<>=+*/;{}.%\[\]] {
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
"for"       return FOR;
"while"     return WHILE;
"if"        return IF;
"else"      return ELSE;
"&&"        return AND;
"||"        return OR;
"break"     return BREAK;
"continue"  return CONTINUE;

\/\/.*\n    ;    /* skip comments */

[ \t\n]+    ;       /* ignore whitespace */

.           yyerror("Unknown character");

%%

int yywrap(void) {
    return 1;
}
