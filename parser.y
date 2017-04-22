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
nodeType *con(const string * value);
nodeType *con(char value);
nodeType *id(const string *, bool isGlobal=false);
nodeType *call(const string *, list<nodeType *> *);
nodeType *func(const string *, list<nodeType*> *, nodeType *);
list<nodeType *> * buildList();
list<nodeType *> * buildList(nodeType *);
list<nodeType *> * buildList(nodeType *, list<nodeType *> *);
void freeNode(nodeType *p);
int ex(nodeType *p, int, int);
void eop();
void run(nodeType* p);
int yylex(void);
void yyerror(char *s);
static map<string, int> variableMap;
static set<int> toBeDefined;
vector<string> reverseLookup;
map<int, int> numVariables;
static int variableCounter;
%}
%debug
%union {
    int iValue;                 /* integer value */
    char cValue;
    const string * sValue;
    const string * variable;
    nodeType *nPtr;             /* node pointer */
    list<nodeType *> *arguments;
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token <variable> VARIABLE
%token FOR WHILE IF PRINT READ BREAK CONTINUE DEF END RETURN
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
        variable                            { $$ = buildList($1);}
        | variable ',' parameterList        { $$ = buildList($1, $3);}
        |                                   { $$ = buildList();}
        ;

argumentList:
        expr                                  { $$ = buildList($1);}
        | expr ',' argumentList               { $$ = buildList($1, $3);}
        |                                     { $$ = buildList();}
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
        | RETURN expr ';'                     { $$ = opr(RETURN, 1, $2);}
        | RETURN ';'                          { $$ = opr(RETURN, 0);}  
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

nodeType *con(const string * value) {
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


list<nodeType *> * buildList() {
    auto p = new list<nodeType*>();
    return p;
}

list<nodeType *> * buildList(nodeType * p) {
    return new list<nodeType*>{p};
}
list<nodeType *> * buildList(nodeType *n , list<nodeType *> *l) {
    l->push_front(n);
    return l;
}

nodeType *func(const string * name, list<nodeType*> *parameters, nodeType *stmts) {
    nodeType * p;
    if ((p = (nodeType*)malloc(SIZEOF_NODETYPE + sizeof(funcNodeType))) == nullptr) {
        cerr << "out of memory" << endl;
    }
    p->type = typeFunc;
    p->func.parameters = parameters;
    p->func.stmts = stmts;
    if (variableMap.count(*name) != 0) {
        if (toBeDefined.count(variableMap[*name]) != 0) {
            p->func.i = variableMap[*name];
            toBeDefined.erase(variableMap[*name]);
        } else {
            cerr << "Redefinition of function: " << *name << endl;
            exit(-1);
        }
    } else {
        p->func.i = variableCounter++;
        variableMap[*name] = p->func.i;
        reverseLookup.push_back(*name);
    }
    return p;
}

nodeType *id(const string * name, bool isGlobal) {
    nodeType * p;
    size_t nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = (nodeType*)malloc(nodeSize)) == NULL) {
        cerr << "out of memory!" << endl;;
    }
    p->type = typeId;
    if (variableMap.count(*name) == 0) {
        p->id.i = variableCounter++;
        variableMap[*name] = p->id.i;
        reverseLookup.push_back(*name);
    } else {
        p->id.i = variableMap[*name];
    }
    p->id.global = isGlobal;
    return p;
}

nodeType *call(const string * name, list<nodeType *> * arguments) {
    nodeType * p;
    if (variableMap.count(*name) == 0) {
        variableMap[*name] = variableCounter++;
        reverseLookup.push_back(*name);
        toBeDefined.insert(variableCounter - 1);
    }
    if ((p = (nodeType*)malloc(SIZEOF_NODETYPE + sizeof(callNodeType))) == nullptr) {
        cerr << "out of memory" << endl;
    }
    p->type = typeCall;
    p->call.i = variableCounter - 1;
    p->call.arguments = arguments;
    return p;
}

void init() {
    for (auto & i: {"gets", "getc", "geti", "puts", "putc", "puti", "puts_", "putc_", "puti_"}) {
        variableMap[i] = variableCounter ++;
        reverseLookup.push_back(i);
    }
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

int main(int argc, const char *argv[]) {
    yydebug = 0;
    extern FILE* yyin;
    init();
    yyin = fopen(argv[1], "r");
    yyparse();
    return 0;
}
