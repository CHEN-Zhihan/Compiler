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
using std::map; using std::pair;

#include <vector>
using std::vector;

#include <set>
using std::set;

#include "c6.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *con(int value);
nodeType *con(const char * value);
nodeType *con(char value);
list<nodeType *> *append(nodeType *);
list<nodeType *> *append(list<nodeType *> *, nodeType *);
nodeType *id(const char *, bool isGlobal=false);
nodeType *append(nodeType *, nodeType *);
nodeType *call(const char *, list<nodeType *> *);
nodeType *func(const char *, list<nodeType*> *, nodeType *);
nodeType *define(const char*, nodeType*);
list<nodeType*> * buildParList();
list<nodeType*> * buildParList(nodeType*);
list<nodeType*> * buildParList(nodeType*, list<nodeType *> *);
list<nodeType *> * buildArgList();
list<nodeType *> * buildArgList(nodeType *);
list<nodeType *> * buildArgList(nodeType *, list<nodeType *> *);
void freeNode(nodeType *p);
int ex(nodeType *p, int, int);
void eop();
void run(nodeType* p);
int yylex(void);
void yyerror(char *s);
static map<string, int> variableMap;
vector<string> reverseLookup;
set<int> functionSet;
static int variableCounter;
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

%type <nPtr> stmt expr stmt_list variable constant
%type <arguments> argumentList parameterList
%%

program:
        stmt_list END                          {run($1); exit(0); }
        ;

parameterList:
        variable                            { $$ = buildParList($1);}
        | variable ',' parameterList        { $$ = buildParList($1, $3);}
        |                                   { $$ = buildParList();}
        ;

argumentList:
        expr                                  { $$ = buildArgList($1);}
        | expr ',' argumentList               { $$ = buildArgList($1, $3);}
        |                                     { $$ = buildArgList();}
        ;

stmt:
          ';'                                 { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                            { $$ = $1;}
        | variable '=' expr ';'                   { $$ = opr('=', 2, $1, $3);}
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = opr(FOR, 4, $3, $4, $5, $7);}
        | WHILE '(' expr ')' stmt             { $$ = opr(WHILE, 2, $3, $5);}
        | IF '(' expr ')' stmt %prec IFX      {  $$ = opr(IF, 2, $3, $5);}
        | IF '(' expr ')' stmt ELSE stmt      { $$ = opr(IF, 3, $3, $5, $7);}
        | '{' stmt_list '}'                   { $$ = $2;}
        | BREAK                               { $$ = opr(BREAK, 0);}
        | CONTINUE                            { $$ = opr(CONTINUE, 0);}
        | DEF VARIABLE '(' parameterList ')' '{' stmt_list '}'    { $$ = func($2, $4, $7);}
        ;

constant:
        INTEGER                     { $$ = con($1);}
        | STRING                    { $$ = con($1);}
        | CHARACTER                 { $$ = con($1);}

variable:
        VARIABLE                { $$ = id($1);}
        | '@' VARIABLE          { $$ = id($2, true);}

stmt_list:
          stmt                  { $$ = $1;}
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2);}
        ;
expr:
        constant                   {$$ = $1;}
        | variable                  { $$ = $1;}
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



nodeType *con(int value) {
    nodeType * p = prepareConstant();
    p->valueType = INT;
    p->con.iValue = value;
    return p;
}

nodeType *con(char value) {
    nodeType * p = prepareConstant();
    p->valueType = CHAR;
    p->con.cValue = value;
    return p;
}

nodeType *con(const char * value) {
    nodeType * p = prepareConstant();
    p->valueType = STR;
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


list<nodeType *> *append(nodeType *p) {
    return new list<nodeType*>{p};
}

list<nodeType *> *append(list<nodeType*> * l, nodeType * p) {
    l->push_back(p);
    return l;
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

nodeType *func(const char * name, list<nodeType*> *parameters, nodeType *stmts) {
    nodeType * p;
    if ((p = (nodeType*)malloc(SIZEOF_NODETYPE + sizeof(funcNodeType))) == nullptr) {
        cerr << "out of memory" << endl;
    }
    p->type = typeFunc;
    p->func.parameters = parameters;
    p->func.stmts = stmts;
    string n = string(name);
    if (variableMap.count(n) != 0) {
        cerr << "Redefinition of function: " << n << endl;
        exit(-1);
    }
    p->func.i = variableCounter++;
    variableMap[n] = p->func.i;
    reverseLookup.push_back(n);
    functionSet.insert(p->func.i);
    return p;
}

nodeType *id(const char * name, bool isGlobal) {
    nodeType * p;
    size_t nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = (nodeType*)malloc(nodeSize)) == NULL) {
        cerr << "out of memory!" << endl;;
    }
    if (string(name) == "c") {
        ;
    }
    p->type = typeId;
    string n = string(name);
    if (variableMap.count(n) == 0) {
        p->id.i = variableCounter++;
        variableMap[n] = p->id.i;
        reverseLookup.push_back(n);
    } else if (functionSet.count(variableMap[n]) != 0) {
        cerr << "Cannot assign value to function: " << n << endl;
        exit(-1);
    } else {
        p->id.i = variableMap[n];
    }
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
    if (p->type == typeCon and p->valueType == STR) {
        free((char*)p->con.sValue);
    }
    free (p);
}

nodeType *call(const char * name, list<nodeType *> * arguments) {
    string n = string(name);
    if (variableMap.count(n) == 0) {
        cerr << "Call on undefined function: " << n << endl;
        exit(-1);
    }
    int index = variableMap[n];
    nodeType * p;
    if ((p = (nodeType*)malloc(SIZEOF_NODETYPE + sizeof(callNodeType))) == nullptr) {
        cerr << "out of memory" << endl;
    }
    p->type = typeCall;
    p->call.i = index;
    p->call.arguments = arguments;
    return p;
}

void init() {
    for (auto & i: {"gets", "getc", "geti", "puts", "putc", "puti", "gets_", "putc_", "puti_"}) {
        variableMap[i] = variableCounter ++;
        functionSet.insert(variableCounter - 1);
        reverseLookup.push_back(i);
    }
}

int main(int argc, const char *argv[]) {
    yydebug = 0;
    extern FILE* yyin;
    init();
    yyin = fopen(argv[1], "r");
    yyparse();
    return 0;
}
