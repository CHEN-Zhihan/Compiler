%{
#include <stdlib.h>
#include "c6.h"
#include "c6.tab.h"
void yyerror(char *);
%}

%%

[a-z]       {
                yylval.sIndex = *yytext - 'a';
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

[-()<>=+*/;{}.%\[\]] {
                return *yytext;
             }
\"[^\"]*\"  {string temp = string(yytext); yylval.sValue = temp.substr(1, temp.size() - 2); return STRING;}
\'[^\']\'   {yylval.cValue = yytext[1]; return CHARACTER;}
">="        return GE;
"<="        return LE;
"=="        return EQ;
"!="        return NE;
"for"       return FOR;
"while"     return WHILE;
"if"        return IF;
"else"      return ELSE;
"print"     return PRINT;
"read"      return READ;
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
