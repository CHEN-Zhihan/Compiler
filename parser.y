%{
#include <list>
using std::list;

#include <iostream>
using std::cout;    using std::endl;
using std::cerr;

#include <string>
using std::string;

#include <unordered_map>
using std::unordered_map; using std::pair;

#include <vector>
using std::vector;

#include <set>
using std::set;


#include <memory>
using std::shared_ptr;  using std::make_shared;
using std::dynamic_pointer_cast;

#include "Node.h"

/* prototypes */
Node* opr(int, const vector<Node*> &);
Node* con(int value);
Node* con(const string * value);
Node* conChar(const string * value);
Node* id(const string *, bool isGlobal=false);
Node* id(const string *, bool isGlobal=false, list<shared_ptr<Node> > * l=nullptr);
Node* call(const string *, list<shared_ptr<Node> >*);
Node* define(const string *, list<shared_ptr<Node> >*, Node*);
Node* declare(const string *, list<shared_ptr<Node> >*);
list<shared_ptr<Node> > * buildList();
list<shared_ptr<Node> > * buildList(shared_ptr<Node>);
void buildList(shared_ptr<Node>, list<shared_ptr<Node> > *);
void run(shared_ptr<Node> p);
int yylex(void);
void yyerror(char *s);
static unordered_map<string, int> nameMap;
set<int> functionSet;
set<int> callSet;
vector<string> reverseLookup;
static int nameCounter;
%}
%debug
%union {
    int iValue;                 /* integer value */
    int dimension;
    bool ref;
    const string * cValue;
    const string * sValue;
    const string * variable;
    const string * t;
    Node * nPtr;             /* node pointer */
    list<shared_ptr<Node> > * lists;
    VarNode * varNode;
    ExprNode * exprNode;
    StmtNode * stmtNode;
    ConNode * conNode;
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token <variable> VARIABLE
%token <t> TYPE
%token FOR WHILE IF PRINT READ BREAK CONTINUE END RETURN DEF INC DEC NEXT
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-' POSINC PREINC POSDEC PREDEC
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt expr variable constant
%type <lists> argList parList subscriptionList stmtList nonEmptyParList nonEmptyArgList
%type <dimension> subscriptions
%type <ref> reference
%%


program:
        stmtList END                          {
                                                    run(shared_ptr<Node>($1));
                                                    exit(0);
                                                }
        ;

parList:
        emptyList                         {$$ = $1;}
        | nonEmptyParList                    {$$ = $1;}
        ;

nonEmptyParList:
        TYPE VARIABLE                           { $$ = buildList(shared_ptr<Node>(id($1)));}
        | parameterList ','  TYPE VARIABLE         { buildList(shared_ptr<Node>(id($1)), $3); $$ = $3;}
        ;

reference:
        | '&'                               {$$ = true;}
        |                                   {$$ = false;}
        ;

argList:
        emptyList                         {$$ = $1;}
        | nonEmptyArgList                   {$$ = $1;}
        ;

emptyList:
        |                                   {$$ = buildList();}
        ;

nonEmptyArgList:
        expr                                  { $$ = buildList(shared_ptr<Node>($1));}
        | expr ',' argumentList               { buildList(shared_ptr<Node>($1), $3); $$ = $3;}
        ;

declare:
        TYPE declareList ';'                   {$$ = declare($1, $2);}
        | TYPE VARIABLE '(' parameterList ')' ';'   {$$ = declare($1, $2, $4);}

declareVar:
        reference variable                            {}
        | reference variable '=' expr                 {}
        ;

declareList:
        declareVar
        | declareList ',' declareVar        {}

subscriptions:
        |                                   {$$ = 0;}
        | '[' ']'                           {$$ = 1;}
        | subscriptions '[' ']'             {$$ = $1 + 1;}
        ;

subscriptionList:
        '[' expr ']'                        {$$ = buildList(shared_ptr<Node>($2));}
        |  '[' expr ']' subscriptionList      {buildList(shared_ptr<Node>($2), $4); $$ = $4;}
        ;
stmt:
          ';'                                 { $$ = opr(';', {}); }
        | expr ';'                            { $$ = $1; dynamic_cast<ExprNode>($$)->inStmt();}
        | variable '=' expr ';'                   { $$ = opr('=', {$1, $3});}
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = opr(FOR, {$3, $4, $5, $7});}
        | WHILE '(' expr ')' stmt             { $$ = opr(WHILE, {$3, $5});}
        | IF '(' expr ')' stmt %prec IFX      {  $$ = opr(IF, {$3, $5});}
        | IF '(' expr ')' stmt ELSE stmt      { $$ = opr(IF, {$3, $5, $7});}
        | '{' stmtList '}'                   { $$ = $2;}
        | BREAK                               { $$ = opr(BREAK, {});}
        | CONTINUE                            { $$ = opr(CONTINUE, {});}
        | RETURN expr ';'                     { $$ = opr(RETURN, {$2});}
        | RETURN ';'                          { $$ = opr(RETURN, {});}  
        | TYPE declareList ';'                   {$$ = declare($1, $2);}
        | TYPE VARIABLE '(' parList ')' ';'   {$$ = declare($1, $2, $4);}
        | TYPE VARIABLE '(' parList ')' '{' stmtList '}'    { $$ = define($1, $2, $4, $7);}
        ;

constant:
        INTEGER                     { $$ = con($1);}
        | STRING                    { $$ = con($1);}
        | CHARACTER                 { $$ = conChar($1);}

variable:
        VARIABLE                { $$ = id($1);}
        | '@' VARIABLE          { $$ = id($2, true);}
        | '@' VARIABLE subscrptionList { $$ = id($2, true, $3);}
        | VARIABLE subscriptionList     {$$ = id($1, false, $2);}
        ;

stmtList:
          stmt                  { $$ =buildList(shared_ptr<Node>($1));}
        | stmtList stmt        {buildList(shared_ptr<Node>($1), $2); $$ = $2;}
        ;
expr:
        constant                   {$$ = $1;}
        | variable                  { $$ = $1;}
        | variable '=' expr     {$$ = opr('=', {$1, $3});}
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, {$2}); }
        | expr '+' expr         { $$ = opr('+', {$1, $3}); }
        | expr '-' expr         { $$ = opr('-', {$1, $3}); }
        | expr '*' expr         { $$ = opr('*', {$1, $3}); }
        | expr '%' expr         { $$ = opr('%', {$1, $3}); }
        | expr '/' expr         { $$ = opr('/', {$1, $3}); }
        | expr '<' expr         { $$ = opr('<', {$1, $3}); }
        | expr '>' expr         { $$ = opr('>', {$1, $3}); }
        | expr  INC             { $$ = opr(POSINC, {$1});}
        | expr DEC             { $$ = opr(POSDEC, {$1});}
        | INC expr             { $$ = opr(PREINC, {$2});}
        | DEC expr             { $$ = opr(PREDEC, {$2});}
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

Node * stmt(int oper, const vector<Node *>* op) {
    auto v = vector<shared_ptr<Node> >();
    for (const auto i: op) {
        v.push_back(shared_ptr<Node>(i));
    }
    return new StmtNode(oper, v);
}

Node * stmt(const list<shared_ptr<Node> >& l) {
    auto v = vector<shared_ptr<Node>>(l.begin(), l.end());
    return new StmtNode(0, v);
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

Node * define(const string * name, list<shared_ptr<Node> > * parameters, Node * stmts) {
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
    auto varPar = list<shared_ptr<VarNode> >();
    for (const auto & i : *parameters) {
        varPar.push_back(dynamic_pointer_cast<VarNode>(i));
    }
    auto p = new FunctionNode(i, varPar, shared_ptr<Node>(stmts));
    delete name;
    delete parameters;
    return p;
}


Node * id(const string * name, bool isGlobal, list<shared_ptr<Node> > * subscriptions) {
    int i;
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    i = nameMap[*name];
    delete name;
    auto s = list<shared_ptr<Node>>();
    for (const auto & i : *subscriptions) {
        s.push_back(i);
    }
    auto v = new VarNode(i, isGlobal, s);
    delete subscriptions;
    return v;
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
