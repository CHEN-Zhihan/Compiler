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
Node* boolean(bool);
Node* id(const string *, bool isGlobal=false, list<shared_ptr<Node> > * l=nullptr);
Node* stmt(int, const vector<Node*>&);
Node* stmt(list<shared_ptr<Node>>*);
Node* call(const string *, list<shared_ptr<Node> >*);
Node* define(VALUE_TYPE, const string *, list<shared_ptr<Node> >*, list<shared_ptr<Node>>*);
Node* parameter(VALUE_TYPE, const string *, int);
list<shared_ptr<Node> > * buildList();
list<shared_ptr<Node> > * buildList(shared_ptr<Node>);
void buildList(list<shared_ptr<Node> > *, shared_ptr<Node>);
void run(shared_ptr<Node> p);
int yylex(void);
void yyerror(char *s);
static unordered_map<string, int> nameMap;
set<int> functionSet;
set<int> callSet;
vector<string> reverseLookup;
static int nameCounter;
constexpr int STMT_LIST = -1;
%}
%debug
%union {
    int iValue;                 /* integer value */
    int dimension;
    bool bValue;
    const string * cValue;
    const string * sValue;
    const string * variable;
    VALUE_TYPE type;
    Node * nPtr;             /* node pointer */
    list<shared_ptr<Node> > * lists;
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token <bValue> BOOLEAN
%token <variable> VARIABLE
%token <type> TYPE
%token FOR WHILE IF BREAK CONTINUE END RETURN INC DEC
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-' POSINC PREINC POSDEC PREDEC
%left '*' '/' '%'
%right '='
%nonassoc UMINUS

%type <nPtr> stmt expr variable constant declareVar
%type <lists> argList parList subscriptionList stmtList nonEmptyParList nonEmptyArgList declareList
%type <dimension> subscriptions nonEmptySubscriptions
%%


program:
        stmtList END                          {
                                                    run(shared_ptr<Node>(stmt($1)));
                                                    exit(0);
                                                }
        ;

parList:
                                 {$$ = buildList();}
        | nonEmptyParList                    {$$ = $1;}
        ;

nonEmptyParList:
        TYPE VARIABLE subscriptions                          { $$ = buildList(shared_ptr<Node>(parameter($1, $2, $3)));}
        | nonEmptyParList ','  TYPE VARIABLE subscriptions        { buildList($1, shared_ptr<Node>(parameter($3, $4, $5))); $$ = $1;}
        ;

argList:
                                 {$$ = buildList();}
        | nonEmptyArgList                   {$$ = $1;}
        ;


nonEmptyArgList:
        expr                                  { $$ = buildList(shared_ptr<Node>($1));}
        | nonEmptyArgList ',' expr                { buildList($1, shared_ptr<Node>($3)); $$ = $1;}
        ;


declareVar:
        variable                            {$$ = new DeclareNode($1);} //TODO implement declare var
        |variable '=' expr                 {$$ = new DeclareNode($1, $3);}
        ;

declareList:
        declareVar                          {$$ = buildList(shared_ptr<Node>($1));}
        | declareList ',' declareVar        {buildList($1, shared_ptr<Node>($3)); $$ = $1;}
        ;
subscriptions:
                                           {$$ = 0;}
        | nonEmptySubscriptions             {$$ = $1;}
        ;

nonEmptySubscriptions:
         '[' ']'                           {$$ = 1;}
        | nonEmptySubscriptions '[' ']'             {$$ = $1 + 1;}
        ;

subscriptionList:
        '[' expr ']'                        {$$ = buildList(shared_ptr<Node>($2));}
        |  '[' expr ']' subscriptionList      {buildList($4, shared_ptr<Node>($2)); $$ = $4;}
        ;
stmt:
          ';'                                 { $$ = stmt(';', {}); }
        | expr ';'                            { $$ = $1; dynamic_cast<ValueNode*>($$)->inStmt();}
        | TYPE declareList ';'                   {$$ = new StmtNode($1, vector<shared_ptr<Node>>($2->begin(), $2->end())); delete $2;} //TODO implement declare
        | FOR '(' stmt stmt stmt ')' stmt     { $$ = stmt(FOR, {$3, $4, $5, $7});}
        | WHILE '(' expr ')' stmt             { $$ = stmt(WHILE, {$3, $5});}
        | IF '(' expr ')' stmt %prec IFX      {  $$ = stmt(IF, {$3, $5});}
        | IF '(' expr ')' stmt ELSE stmt      { $$ = stmt(IF, {$3, $5, $7});}
        | '{' stmtList '}'                    { $$ = stmt($2);}
        | BREAK                               { $$ = stmt(BREAK, {});}
        | CONTINUE                            { $$ = stmt(CONTINUE, {});}
        | RETURN expr ';'                     { $$ = stmt(RETURN, {$2});}
        | RETURN ';'                          { $$ = stmt(RETURN, {});}  
        | TYPE VARIABLE '(' parList ')' '{' stmtList '}'    { $$ = define($1, $2, $4, $7);}
        ;

constant:
        INTEGER                     { $$ = con($1);}
        | STRING                    { $$ = con($1);}
        | CHARACTER                 { $$ = conChar($1);}
        | BOOLEAN                   { $$ = boolean($1);}
        ;
variable:
        VARIABLE                { $$ = id($1);}
        | '@' VARIABLE          { $$ = id($2, true);}
        | '@' VARIABLE subscriptionList { $$ = id($2, true, $3);}
        | VARIABLE subscriptionList     {$$ = id($1, false, $2);}
        ;

stmtList:
          stmt                  { $$ =buildList(shared_ptr<Node>($1));}
        | stmtList stmt        {buildList($1, shared_ptr<Node>($2)); $$ = $1;}
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
        | variable  INC             { $$ = opr(POSINC, {$1});}
        | variable DEC             { $$ = opr(POSDEC, {$1});}
        | INC variable             { $$ = opr(PREINC, {$2});}
        | DEC variable             { $$ = opr(PREDEC, {$2});}
        | expr GE expr          { $$ = opr(GE, {$1, $3}); }
        | expr LE expr          { $$ = opr(LE, {$1, $3}); }
        | expr NE expr          { $$ = opr(NE, {$1, $3}); }
        | expr EQ expr          { $$ = opr(EQ, {$1, $3}); }
        | expr AND expr            { $$ = opr(AND, {$1, $3}); }
        | expr OR expr            { $$ = opr(OR, {$1, $3}); }
        | '(' expr ')'          { $$ = $2; }
        | VARIABLE '(' argList ')' {$$ = call($1, $3);}
        ;
%%


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

Node * boolean(bool b) {
    return new BoolNode(b);
}

Node * opr(int oper, const vector<Node *>& op) {
    auto v = vector<shared_ptr<Node> >();
    for (const auto i: op) {
        v.push_back(shared_ptr<Node>(i));
    }
    return new ExprNode(oper, v);
}


Node * stmt(int oper, const vector<Node *>& op) {
    auto v = vector<shared_ptr<Node> >();
    for (const auto i: op) {
        v.push_back(shared_ptr<Node>(i));
    }
    return new StmtNode(oper, v);
}
Node* stmt(list<shared_ptr<Node>>* stmts) {
    auto v = vector<shared_ptr<Node> >(stmts->begin(), stmts->end());
    delete stmts;
    return new StmtNode(STMT_LIST, v);
}
list<shared_ptr<Node> > * buildList() {
    return new list<shared_ptr<Node> >();
}

list<shared_ptr<Node> > * buildList(shared_ptr<Node> p) {
    auto l = new list<shared_ptr<Node> >();
    l->push_back(p);
    return l;
}
void buildList(list<shared_ptr<Node> > * l, shared_ptr<Node>n) {
    l->push_back(n);
}

int getID(const string * name) {
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    return nameMap[*name];
}

Node * define(VALUE_TYPE t, const string * name, list<shared_ptr<Node> > * parameters, list<shared_ptr<Node> > * stmts) {
    int i = getID(name);
    if (functionSet.count(i) != 0) {
        cerr << "Redefinition of function: " << *name << endl;
        exit(-1);
    }
    auto varPar = list<shared_ptr<VarNode> >();
    for (const auto & i : *parameters) {
        varPar.push_back(dynamic_pointer_cast<VarNode>(i));
    }
    auto v = vector<shared_ptr<Node>>(stmts->begin(), stmts->end());
    auto p = new FunctionNode(t, i, varPar, v);
    delete name;
    delete parameters;
    delete stmts;
    return p;
}


Node * id(const string * name, bool isGlobal, list<shared_ptr<Node> > * subscriptions) {
    int i = getID(name);
    delete name;
    auto s = list<shared_ptr<Node>>();
    if (subscriptions != nullptr) {
        for (const auto & i : *subscriptions) {
            s.push_back(i);
        }
    }
    auto v = new VarNode(i, isGlobal, s);
    delete subscriptions;
    return v;
}


Node * call(const string * name, list<shared_ptr<Node> > * arguments) {
    int i = getID(name);
    callSet.insert(i);
    auto p = new CallNode(i, *arguments);
    delete name;
    delete arguments;
    return p;
}

Node* parameter(VALUE_TYPE t, const string * name, int dimension) {
    int i = getID(name);
    delete name;
    return new ParameterNode(t, i, dimension);
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
