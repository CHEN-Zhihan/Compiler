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

#include <vector>
using std::vector;

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
void run(list<nodeType*>* p);
int yylex(void);
void yyerror(char *s);

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

%type <nPtr> stmt expr stmt_list variable function definition constant
%type <arguments> argumentList parameterList definitionList
%%

program:
        definitionList END                          {run($1); exit(0); }
        ;

definitionList:
        definition                          { $$ = append($1);}
        | definitionList definition         {$$ = append($1, $2);}
        ;
definition:
        variable '=' constant ';'           { $$ = opr('=', 2, $1, $3); }
        | function                          { $$ = $1;}
        ;

parameterList:
        variable                            { $$ = buildParList($1);}
        | variable ',' parameterList        { $$ = buildParList($1, $3);}
        |                                   { $$ = buildParList();}
        ;
function:
        DEF VARIABLE '(' parameterList ')' '{' stmt_list '}'    { $$ = func($2, $4, $7);}
        ;

argumentList:
        expr                                  { $$ = buildArgList($1);}
        | expr ',' argumentList               { $$ = buildArgList($1, $3);}
        |                                     { $$ = buildArgList();}
        ;

stmt:
          ';'                                 { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                            { $$ = $1;}
        | PRINT expr ';'                      { $$ = opr(PRINT, 1, $2); }
        | variable '=' expr ';'                   { $$ = opr('=', 2, $1, $3);}
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = opr(FOR, 4, $3, $4, $5, $7); }
        | WHILE '(' expr ')' stmt             { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX      {  $$ = opr(IF, 2, $3, $5);}
        | IF '(' expr ')' stmt ELSE stmt      { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'                   { $$ = $2; }
        | BREAK                               { $$ = opr(BREAK, 0); }
        | CONTINUE                            { $$ = opr(CONTINUE, 0); }
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
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
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
static map<int, string> opers{{'+', "+"}, {'-', "-"}, {'*', "*"}, {'/', "/"}, 
                                    {'%', "%"}, {'<', "<"}, {'>', ">"}, {GE, ">="}, {NE, "!="}, {LE, "<="}, {EQ, "=="}};
static map<int, string> consts{{INT, "integer"}, {CHAR, "character"}, {STR, "string"}};
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
constType getType(const nodeType * n) {
    if (n->type == typeCon) {
        return n->con.type;
    } else {
        return INT;
    }
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
    if (opers.count(oper) != 0) {
        auto left = getType(p->opr.op[0]);
        auto right = getType(p->opr.op[1]);
        if (left != right) {
            cerr << "Invalid operation: " << opers[oper] << " between " << consts[left] << " and " << consts[right] << endl;
            exit(-1);
        }

    }

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
    p->func.name = new string(name);
    return p;
}

nodeType *id(const char * name, bool isGlobal) {
    nodeType * p;
    size_t nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = (nodeType*)malloc(nodeSize)) == NULL) {
        cerr << "out of memory!" << endl;;
    }
    p->type = typeId;
    p->id.name = new string(name);
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

    return arguments->front();
}


void init() {
    ;
}

int main(int argc, const char *argv[]) {
    yydebug = 0;
    init();
    extern FILE* yyin;
    yyin = fopen(argv[1], "r");
    yyparse();
    return 0;
}
