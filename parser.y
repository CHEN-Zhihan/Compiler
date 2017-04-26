%{
#include <list>
using std::list;

#include <iostream>
using std::cout;    using std::endl;
using std::cerr;

#include <string>
using std::string;

#include <map>
using std::map; using std::pair;

#include <vector>
using std::vector;

#include <set>
using std::set;

#include "c6.h"

#define v new vector<nodeType*>

/* prototypes */
nodeType *opr(int, int, nodeType* *);
nodeType *con(int value);
nodeType *con(const string * value);
nodeType *conChar(const string * value);
nodeType *id(const string *, bool isGlobal=false);
nodeType *call(const string *, list<nodeType *> *);
nodeType *func(const string *, list<nodeType*> *, nodeType *);
list<nodeType *> * buildList();
list<nodeType *> * buildList(nodeType *);
list<nodeType *> * buildList(nodeType *, list<nodeType *> *);
void run(nodeType* p);
void freeNode(nodeType*p);
int yylex(void);
void yyerror(char *s);
static map<string, int> nameMap;
set<int> functionSet;
map<int, set<int> > variableTable;
set<int> callSet;
vector<string> reverseLookup;
static int nameCounter;
%}
%debug
%union {
    int iValue;                 /* integer value */
    const string * cValue;
    const string * sValue;
    const string * variable;
    nodeType *nPtr;             /* node pointer */
    list<nodeType *> *lists;
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token <variable> VARIABLE
%token FOR WHILE IF PRINT READ BREAK CONTINUE END RETURN DEF
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list variable constant
%type <lists> argumentList parameterList
%%



program:
        stmt_list END                          {
                                                    run($1);
                                                    freeNode($1);
                                                    exit(0);
                                                }
        ;

parameterList:
        VARIABLE                            { $$ = buildList(id($1));}
        | VARIABLE ',' parameterList        { $$ = buildList(id($1), $3);}
        |                                   { $$ = buildList();}
        ;

argumentList:
        expr                                  { $$ = buildList($1);}
        | expr ',' argumentList               { $$ = buildList($1, $3);}
        |                                     { $$ = buildList();}
        ;

stmt:
          ';'                                 { $$ = opr(';', 0, nullptr); }
        | expr ';'                            { $$ = $1;}
        | variable '=' expr ';'                   { $$ = opr('=', 2, new nodeType*[2]{$1, $3});}
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = opr(FOR, 4, new nodeType*[4]{$3, $4, $5, $7});}
        | WHILE '(' expr ')' stmt             { $$ = opr(WHILE, 2, new nodeType*[2]{$3, $5});}
        | IF '(' expr ')' stmt %prec IFX      {  $$ = opr(IF, 2, new nodeType*[2]{$3, $5});}
        | IF '(' expr ')' stmt ELSE stmt      { $$ = opr(IF, 3, new nodeType*[3]{$3, $5, $7});}
        | '{' stmt_list '}'                   { $$ = $2;}
        | BREAK                               { $$ = opr(BREAK, 0, nullptr);}
        | CONTINUE                            { $$ = opr(CONTINUE, 0, nullptr);}
        | RETURN expr ';'                     { $$ = opr(RETURN, 1, new nodeType*[1]{$2});}
        | RETURN ';'                          { $$ = opr(RETURN, 0, nullptr);}  
        | DEF VARIABLE '(' parameterList ')' '{' stmt_list '}'    { $$ = func($2, $4, $7);}
        ;

constant:
        INTEGER                     { $$ = con($1);}
        | STRING                    { $$ = con($1);}
        | CHARACTER                 { $$ = conChar($1);}

variable:
        VARIABLE                { $$ = id($1);}
        | '@' VARIABLE          { $$ = id($2, true);}

stmt_list:
          stmt                  { $$ = $1;}
        | stmt_list stmt        { $$ = opr(';', 2, new nodeType*[2]{$1, $2});}
        ;
expr:
        constant                   {$$ = $1;}
        | variable                  { $$ = $1;}
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, new nodeType*[1]{$2}); }
        | expr '+' expr         { $$ = opr('+', 2, new nodeType*[2]{$1, $3}); }
        | expr '-' expr         { $$ = opr('-', 2, new nodeType*[2]{$1, $3}); }
        | expr '*' expr         { $$ = opr('*', 2, new nodeType*[2]{$1, $3}); }
        | expr '%' expr         { $$ = opr('%', 2, new nodeType*[2]{$1, $3}); }
        | expr '/' expr         { $$ = opr('/', 2, new nodeType*[2]{$1, $3}); }
        | expr '<' expr         { $$ = opr('<', 2, new nodeType*[2]{$1, $3}); }
        | expr '>' expr         { $$ = opr('>', 2, new nodeType*[2]{$1, $3}); }
        | expr GE expr          { $$ = opr(GE, 2, new nodeType*[2]{$1, $3}); }
        | expr LE expr          { $$ = opr(LE, 2, new nodeType*[2]{$1, $3}); }
        | expr NE expr          { $$ = opr(NE, 2, new nodeType*[2]{$1, $3}); }
        | expr EQ expr          { $$ = opr(EQ, 2, new nodeType*[2]{$1, $3}); }
        | expr AND expr            { $$ = opr(AND, 2, new nodeType*[2]{$1, $3}); }
        | expr OR expr            { $$ = opr(OR, 2, new nodeType*[2]{$1, $3}); }
        | '(' expr ')'          { $$ = $2; }
        | VARIABLE '(' argumentList ')' {$$ = call($1, $3);}
        ;
%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)




nodeType *con(int value) {
    return new nodeType(nodeCon, conNodeType(INT, value));
}

nodeType *conChar(const string * value) {
    return new nodeType(nodeCon, conNodeType(CHAR, value, false));
}

nodeType *con(const string * value) {
    return new nodeType(nodeCon, conNodeType(STR, value, true));
}

nodeType *opr(int oper, int nop, nodeType** op) {
    nodeType * p = new nodeType(nodeOpr, oprNodeType(oper, nop, op));
    if (p == nullptr) {
        cerr << "out of memory" << endl;
    }
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
    int i;
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    i = nameMap[*name];
    if (functionSet.count(i) != 0) {
        cerr << "Redefinition of function: " << *name << endl;
        exit(-1);
    }
    nodeType * p = new nodeType(nodeId, idNodeType(i, parameters, stmts, i));
    if (p == nullptr) {
        cerr << "out of memory" << endl;
    }
    delete name;

    return p;
}

nodeType *id(const string * name, bool isGlobal) {
    int i;
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    i = nameMap[*name];
    nodeType * p = new nodeType(nodeId, idNodeType(i, isGlobal));
    if (p == nullptr) {
        cerr << "out of memory" << endl;
    }
    delete name;
    return p;
}

nodeType *call(const string * name, list<nodeType *> * arguments) {
    int i;
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    i = nameMap[*name];
    callSet.insert(i);
    nodeType * p = new nodeType(nodeId, idNodeType(i, arguments, i));
    if (p == nullptr) {
        cerr << "out of memory" << endl;
    }
    delete name;
    return p;
}

void init() {
    for (auto & i: {"gets", "getc", "geti", "puts", "putc", "puti", "puts_", "putc_", "puti_"}) {
        nameMap[i] = nameCounter++;
        functionSet.insert(nameCounter - 1);
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
