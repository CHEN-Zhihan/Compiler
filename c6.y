%{
#include <list>
using std::list;
#include <iostream>
using std::printf;  using std::cout;
using std::fprintf; using std::endl;
#include <cstdlib>
using std::malloc;
#include <cstdarg>
#include <cstring>
using std::strlen;
#include <string>
using std::string;
#include <map>
using std::map;


#include "c6.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *con(int value);
nodeType *con(const char * value);
nodeType *con(char value);
nodeType *id(const char *);
nodeType *append(nodeType *, nodeType *);
nodeType *func(const char *, list<string> *, nodeType *);
list<string> * buildList();
list<string> * buildList(const char *);
list<string> * buildList(const char *, list<string> *);
void freeNode(nodeType *p);
int ex(nodeType *p, int, int);
void eop();
int yylex(void);

void yyerror(char *s);
static int variableCounter = 0;
map<string, int> variableMap;
%}

%union {
    int iValue;                 /* integer value */
    char cValue;
    const char * sValue;
    const char * variable;
    nodeType *nPtr;             /* node pointer */
    std::list<string> * parameterPointer;
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token <variable> VARIABLE
%token FOR WHILE IF PRINT READ BREAK CONTINUE DEF
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list lval rval variable definitionList function definition
%type <parameterPointer> parameterList
%%

program:
        definitionList                          { eop(); exit(0); }
        | 

definitionList:
        definition                          { $$ = $1;}
        | definition definitionList        { $$ = append($1, $2);}

definition:
        lval '=' rval ';'                   { $$ = opr('=', 2, $1, $3);}
        | function                          { $$ = $1;}
        ;

parameterList:
        VARIABLE                            { $$ = buildList($1);}
        | VARIABLE ',' parameterList        { $$ = buildList($1, $3);}
        |                                   { $$ = buildList();}
        ;
function:
        DEF VARIABLE '(' parameterList ')' '{' stmt_list '}'    { $$ = func($2, $4, $7);}
        ;


stmt:
          ';'                                 { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                            { $$ = $1; }
        | PRINT expr ';'                      { $$ = opr(PRINT, 1, $2); }
        | lval '=' rval ';'                   { $$ = opr('=', 2, $1, $3);}
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = opr(FOR, 4, $3, $4, $5, $7); }
        | WHILE '(' expr ')' stmt             { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX      { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt      { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'                   { $$ = $2; }
        | BREAK                               { $$ = opr(BREAK, 0); }
        | CONTINUE                            { $$ = opr(CONTINUE, 0); }
        ;

variable:VARIABLE               { $$ = id($1);}
    ;

rval:
    variable                    { $$ = $1;}
    ;

lval:
    variable                    { $$ = $1;}
    ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | STRING                { $$ = con($1); }
        | CHARACTER             { $$ = con($1); }
        | rval                  { $$ = $1;}
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

nodeType *con(const char * value) {
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

nodeType * append(nodeType * head, nodeType * list) {
    return head;
}

list<string> * buildList() {
    return new list<string>();
}

list<string> * buildList(const char * parameter) {
    return new list<string>{string(parameter)};
}

list<string> * buildList(const char * parameter, list<string> * l) {
    l->push_front(string(parameter));
    return l;
}

nodeType *func(const char * name, list<string> *parameters, nodeType *stmts) {
    return stmts;
}

nodeType *id(const char * name) {
    int index = variableMap[string(name)] = variableCounter++;
    nodeType * p;
    size_t nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = (nodeType*)malloc(nodeSize)) == NULL) {
        yyerror("out of memory!");
    }
    p->type = typeId;
    p->id.i = index;
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    if (p->type == typeCon and p->con.type == STR) {
        free((char*)p->con.sValue);
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
