%{
#include <list>
using std::list;

#include <iostream>
using std::printf;  using std::cout;
using std::fprintf; using std::endl;
using std::cerr;

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
nodeType *id(const char *, bool isGlobal=false);
nodeType *append(nodeType *, nodeType *);
nodeType *call(const char *, list<nodeType *> *);
nodeType *func(nodeType *, list<nodeType*> *, nodeType *);
list<nodeType*> * buildParList();
list<nodeType*> * buildParList(nodeType*);
list<nodeType*> * buildParList(nodeType*, list<nodeType *> *);
list<nodeType *> * buildArgList();
list<nodeType *> * buildArgList(nodeType *);
list<nodeType *> * buildArgList(nodeType *, list<nodeType *> *);
void freeNode(nodeType *p);
int ex(nodeType *p, int, int);
void eop();
int yylex(void);

void yyerror(char *s);
static int variableCounter = 0;
map<string, nodeType*> variableMap;
map<string, nodeType*> functionMap;
%}
%debug
%union {
    int iValue;                 /* integer value */
    char cValue;
    const char * sValue;
    const char * variable;
    nodeType *nPtr;             /* node pointer */
    list<nodeType *> *arguments;
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token <variable> VARIABLE
%token FOR WHILE IF PRINT READ BREAK CONTINUE DEF END
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list variable definitionList function definition
%type <arguments> argumentList parameterList
%%

program:
        definitionList END                          {eop(); exit(0); }
        ;

definitionList:
        definition                          { $$ = $1; cout << "definitionList -> definition" << endl;}
        | definitionList definition         { $$ = append($1, $2); cout << "definitionList -> definition definitionList" << endl;}
        ;
definition:
        VARIABLE '=' outExpr ';'                   { $$ = define(); cout << "definition -> =" << endl;}
        | function                          { $$ = $1; cout << "definition -> function" << endl;}
        ;

parameterList:
        variable                            { $$ = buildParList($1);}
        | variable ',' parameterList        { $$ = buildParList($1, $3);}
        |                                   { $$ = buildParList();}
        ;
function:
        DEF variable '(' parameterList ')' '{' stmt_list '}'    { $$ = func($2, $4, $7); cout << "function -> DEF" << endl;}
        ;

argumentList:
        expr                                  { $$ = buildArgList($1);}
        | expr ',' argumentList               { $$ = buildArgList($1, $3);}
        |                                     { $$ = buildArgList();}
        ;

stmt:
          ';'                                 { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                            { $$ = $1; cout << "stmt -> expr" << endl;}
        | PRINT expr ';'                      { $$ = opr(PRINT, 1, $2); }
        | variable '=' expr ';'                   { $$ = opr('=', 2, $1, $3); cout << "stmt -> =" << endl;}
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = opr(FOR, 4, $3, $4, $5, $7); }
        | WHILE '(' expr ')' stmt             { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX      {  cout << "stmt -> IF" << endl; $$ = opr(IF, 2, $3, $5);}
        | IF '(' expr ')' stmt ELSE stmt      { $$ = opr(IF, 3, $3, $5, $7); cout << "stmt -> IF ELSE" << endl;}
        | '{' stmt_list '}'                   { $$ = $2; }
        | BREAK                               { $$ = opr(BREAK, 0); }
        | CONTINUE                            { $$ = opr(CONTINUE, 0); }
        ;

variable:
        VARIABLE                { $$ = id($1);}
        | '@' VARIABLE          { $$ = id($1, true)}

stmt_list:
          stmt                  { $$ = $1; cout << "stmt_list -> stmt" << endl;}
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); cout << "stmt_list -> stmt_list stmt" << endl;}
        ;

outExpr:
          INTEGER               { $$ = con($1); cout << "outExpr -> INTEGER" << endl;}
        | STRING                { $$ = con($1); }
        | CHARACTER             { $$ = con($1); }
        | variable                  { $$ = $1;cout << "outExpr -> variable" << endl;}
        | '-' outExpr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | outExpr '+' outExpr         { $$ = opr('+', 2, $1, $3); }
        | outExpr '-' outExpr         { $$ = opr('-', 2, $1, $3); }
        | outExpr '*' outExpr         { $$ = opr('*', 2, $1, $3); }
        | outExpr '%' outExpr         { $$ = opr('%', 2, $1, $3); }
        | outExpr '/' outExpr         { $$ = opr('/', 2, $1, $3); }
        | outExpr '<' outExpr         { $$ = opr('<', 2, $1, $3); }
        | outExpr '>' outExpr         { $$ = opr('>', 2, $1, $3); }
        | outExpr GE outExpr          { $$ = opr(GE, 2, $1, $3); }
        | outExpr LE outExpr          { $$ = opr(LE, 2, $1, $3); }
        | outExpr NE outExpr          { $$ = opr(NE, 2, $1, $3); }
        | outExpr EQ outExpr          { $$ = opr(EQ, 2, $1, $3); }
        | outExpr AND outExpr            { $$ = opr(AND, 2, $1, $3); }
        | outExpr OR outExpr            { $$ = opr(OR, 2, $1, $3); }
        | '(' outExpr ')'          { $$ = $2; }
        | VARIABLE '(' argumentList ')' {$$ = call($1, $3);}
        ;

expr:
          INTEGER               { $$ = con($1); cout << "expr -> INTEGER" << endl;}
        | STRING                { $$ = con($1); }
        | CHARACTER             { $$ = con($1); }
        | variable                  { $$ = $1;cout << "expr -> variable" << endl;}
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
        | VARIABLE '(' argumentList ')' {$$ = call($1, $3);}
        ;
%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType * prepareConstant() {
    nodeType * p;
    size_t nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
    if ((p = (nodeType*)malloc(nodeSize)) == nullptr) {
        cerr << "out of memory!" << endl;;
    }
    p->type = typeCon;
    return p;
}

nodeType * define(nodeType * )

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
        cerr << "out of memory" << endl;

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

nodeType * append(nodeType * list, nodeType * head) {
    return head;
}

list<nodeType *> * buildParList() {
    return new list<nodeType *>();
}

list<nodeType *> * buildParList(nodeType * parameter) {
    return new list<nodeType *>{parameter};
}

list<nodeType *> * buildParList(nodeType * parameter, list<nodeType*> * l) {
    l->push_front(parameter);
    return l;
}

list<nodeType *> * buildArgList() {
    auto p = new list<nodeType*>();
    return p;
}

list<nodeType *> * buildArgList(nodeType * p) {
    return new list<nodeType*>{p};
}
list<nodeType *> * buildArgList(nodeType *n , list<nodeType *> *l) {
    l->push_front(n);
    return l;
}

nodeType *func(nodeType * name, list<nodeType*> *parameters, nodeType *stmts) {
    return stmts;
}

nodeType *id(const char * name, bool isGlobal) {
    nodeType * p;
    size_t nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = (nodeType*)malloc(nodeSize)) == NULL) {
        cerr << "out of memory!" << endl;;
    }
    p->type = typeId;
    p->id.i = variableCounter++;
    p->id.global = isGlobal;
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

nodeType *call(const char * function, list<nodeType *> * arguments) {
    if (functionMap.count(string(function)) == 0) {
        cerr << "Reference to undefined function: " << string(function) << endl;
    }

    return arguments->front();
}


void init() {
    ;
}

int main(int argc, char **argv) {
    yydebug = 1;
    init();
    extern FILE* yyin;
    yyin = fopen(argv[1], "r");
    yyparse();
    return 0;
}
