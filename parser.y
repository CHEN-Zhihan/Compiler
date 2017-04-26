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


#include <memory>
using std::shared_ptr;  using std::make_shared;

#include "Node.h"

/* prototypes */
Node* opr(int, const vector<Node*> &);
Node* con(int value);
Node* con(const string * value);
Node* conChar(const string * value);
Node* id(const string *, bool isGlobal=false);
Node* call(const string *, list<shared_ptr<Node> >*);
Node* func(const string *, list<shared_ptr<Node> >*, Node*);
list<shared_ptr<Node> > * buildList();
list<shared_ptr<Node> > * buildList(shared_ptr<Node>);
void buildList(shared_ptr<Node>, list<shared_ptr<Node> > *);
void run(shared_ptr<Node> p);
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
    Node * nPtr;             /* node pointer */
    list<shared_ptr<Node> > * lists;
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
                                                    run(shared_ptr<Node>($1));
                                                    exit(0);
                                                }
        ;

parameterList:
        VARIABLE                            { $$ = buildList(shared_ptr<Node>(id($1)));}
        | VARIABLE ',' parameterList        { buildList(shared_ptr<Node>(id($1)), $3); $$ = $3;}
        |                                   { $$ = buildList();}
        ;

argumentList:
        expr                                  { $$ = buildList(shared_ptr<Node>($1));}
        | expr ',' argumentList               { buildList(shared_ptr<Node>($1), $3); $$ = $3;}
        |                                     { $$ = buildList();}
        ;

stmt:
          ';'                                 { $$ = opr(';', {}); }
        | expr ';'                            { $$ = $1;}
        | variable '=' expr ';'                   { $$ = opr('=', {$1, $3});}
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = opr(FOR, {$3, $4, $5, $7});}
        | WHILE '(' expr ')' stmt             { $$ = opr(WHILE, {$3, $5});}
        | IF '(' expr ')' stmt %prec IFX      {  $$ = opr(IF, {$3, $5});}
        | IF '(' expr ')' stmt ELSE stmt      { $$ = opr(IF, {$3, $5, $7});}
        | '{' stmt_list '}'                   { $$ = $2;}
        | BREAK                               { $$ = opr(BREAK, {});}
        | CONTINUE                            { $$ = opr(CONTINUE, {});}
        | RETURN expr ';'                     { $$ = opr(RETURN, {$2});}
        | RETURN ';'                          { $$ = opr(RETURN, {});}  
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
        | stmt_list stmt        { $$ = opr(';', {$1, $2});}
        ;
expr:
        constant                   {$$ = $1;}
        | variable                  { $$ = $1;}
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, {$2}); }
        | expr '+' expr         { $$ = opr('+', {$1, $3}); }
        | expr '-' expr         { $$ = opr('-', {$1, $3}); }
        | expr '*' expr         { $$ = opr('*', {$1, $3}); }
        | expr '%' expr         { $$ = opr('%', {$1, $3}); }
        | expr '/' expr         { $$ = opr('/', {$1, $3}); }
        | expr '<' expr         { $$ = opr('<', {$1, $3}); }
        | expr '>' expr         { $$ = opr('>', {$1, $3}); }
        | expr GE expr          { $$ = opr(GE, {$1, $3}); }
        | expr LE expr          { $$ = opr(LE, {$1, $3}); }
        | expr NE expr          { $$ = opr(NE, {$1, $3}); }
        | expr EQ expr          { $$ = opr(EQ, {$1, $3}); }
        | expr AND expr            { $$ = opr(AND, {$1, $3}); }
        | expr OR expr            { $$ = opr(OR, {$1, $3}); }
        | '(' expr ')'          { $$ = $2; }
        | VARIABLE '(' argumentList ')' {$$ = call($1, $3);}
        ;
%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)




Node * con(int value) {
    return new IntNode(value);
}

Node * conChar(const string * value) {
    auto p = new CharNode(*value);
    delete value;
    return p;
}

Node * con(const string * value) {
    auto p = new StrNode(*value);
    delete value;
    return p;
}

Node * opr(int oper, const vector<Node *>& op) {
    auto v = vector<shared_ptr<Node> >();
    for (const auto i: op) {
        v.push_back(shared_ptr<Node>(i));
    }
    return new OprNode(oper, v);
}


list<shared_ptr<Node> > * buildList() {
    return new list<shared_ptr<Node> >();
}

list<shared_ptr<Node> > * buildList(shared_ptr<Node> p) {
    auto l = new list<shared_ptr<Node> >();
    l->push_back(p);
    return l;
}
void buildList(shared_ptr<Node>n , list<shared_ptr<Node> > * l) {
    l->push_front(n);
}

Node * func(const string * name, list<shared_ptr<Node> > * parameters, Node * stmts) {
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
    auto p = new FunctionNode(i, *parameters, shared_ptr<Node>(stmts));
    delete name;
    delete parameters;
    return p;
}

Node * id(const string * name, bool isGlobal) {
    int i;
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    i = nameMap[*name];
    delete name;
    return new VarNode(i, isGlobal);
}

Node * call(const string * name, list<shared_ptr<Node> > * arguments) {
    int i;
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    i = nameMap[*name];
    callSet.insert(i);
    auto p = new CallNode(i, *arguments);
    delete name;
    delete arguments;
    return p;
}

void init() {
    for (auto i: {"gets", "getc", "geti", "puts", "putc", "puti", "puts_", "putc_", "puti_"}) {
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
