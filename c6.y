%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include "c6.h"

using std::string;
/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *con(int value);
nodeType *con(const string& value);
nodeType *con(char value);
void freeNode(nodeType *p);
int ex(nodeType *p, int, int);
void eop();
int yylex(void);

void yyerror(char *s);
%}

%union {
    int iValue;                 /* integer value */
    char cValue;
    string sValue;
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token FOR WHILE IF PRINT READ BREAK CONTINUE OFFSET RVALUE
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list lval rval vari

%%

program:
        function                { eop(); exit(0); }
        ;

function:
          function stmt         { ex($2,998,998); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                                 { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                            { $$ = $1; }
        | PRINT expr ';'                      { $$ = opr(PRINT, 1, $2); }
        | READ lval ';'                       { $$ = opr(READ, 1, $2); }
        | lval '=' expr ';'                   { $$ = opr('=', 2, $1, $3); }
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = opr(FOR, 4, $3, $4, $5, $7); }
        | WHILE '(' expr ')' stmt             { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX      { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt      { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'                   { $$ = $2; }
        | BREAK                               { $$ = opr(BREAK, 0); }
        | CONTINUE                            { $$ = opr(CONTINUE, 0); }
        ;

vari:
          VARIABLE { $$ = nullptr; }
        ;

lval:
      vari               { $$ = $1; }
    | vari '[' expr ']'  { $$ = opr(OFFSET, 2, $1, $3); }
    ;

rval:
      vari               { $$ = $1; }
    | vari '[' expr ']'  { $$ = opr(RVALUE, 1, opr(OFFSET, 2, $1, $3)); }
    ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | STRING                { $$ = con($1); }
        | CHARACTER             { $$ = con($1); }
        | rval                     { $$ = $1; }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '%' expr         { $$ = opr('%', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
        | expr AND expr            { $$ = opr(AND, 2, $1, $3); }
        | expr OR expr            { $$ = opr(OR, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;

%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType * prepareConstant() {
    nodeType * p;
    size_t nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
    if ((p = (nodeType*)malloc(nodeSize)) == nullptr) {
        yyerror("out of memory!");
    }
    p->type = typeCon;
    return p;
}

nodeType *con(int value) {
    nodeType * p = prepareConstant();
    p->con.type = INT;
    p->con.iValue = value;
    return p;
}

nodeType *con(char value) {
    nodeType * p = prepareConstant();
    p->con.type = CHAR;
    p->con.cValue = value;
    return p;
}

nodeType *con(const string & value) {
    nodeType * p = prepareConstant();
    p->con.type = STR;
    p->con.sValue = value;
    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
        (nops - 1) * sizeof(nodeType*);
    if ((p = (nodeType*)malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

int main(int argc, char **argv) {
extern FILE* yyin;
    yyin = fopen(argv[1], "r");
    yyparse();
    return 0;
}
