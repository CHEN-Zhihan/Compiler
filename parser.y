%{
#include <vector>
using std::vector;

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
ExprNode* expr(int, const vector<Node*> &);
Node* stmt(int, const vector<Node*>&);
Node* con(int value);
Node* con(const string * value);
Node* conChar(const string * value);
Node* id(const string *, bool isGlobal=false);
Node* id(Node *, vector<Node*> *);
Node* call(const string *, vector<Node*>*);
Node* func(const string *, vector<Node*>*, vector<Node*>*);
void run(shared_ptr<Node> p);
int yylex(void);
void yyerror(char *s);
static unordered_map<string, int> nameMap;
set<int> functionSet;
set<int> callSet;
vector<string> reverseLookup;
static int nameCounter;
const int STMTLIST = -1;
const int EXPR = -2;
const int CALL = -3;
const int FUNCTION = -4;
%}
%debug
%union {
    int iValue;                 /* integer value */
    const string * cValue;
    const string * sValue;
    const string * variable;
    bool bValue;
    Node * nPtr;             /* node pointer */
    ExprNode * exprNode;
    vector<VarNode*>* varlist;
    vector<ExprNode*>* exprList;
    vector<StmtNode*>* stList;
    vector<DeclareNode*>* declList;
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token <variable> VARIABLE
%token FOR WHILE IF PRINT READ BREAK CONTINUE END RETURN DEF INC DEC ARRAY
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-' POSINC PREINC POSDEC PREDEC
%left '*' '/' '%'
%nonassoc UMINUS

%type <nPtr> stmt variable constant
%type <exprNode> expr
%type <varList> parameterList
%type <declList> declareList
%type <exprList> subscriptionList argumentList
%type <stList> stmtList
%%



program:
        stmtList END                          {
                                                    run(stmt(STMT_LIST, *$1));
                                                    delete $1;
                                                    exit(0);
                                                }
        ;

parameterList:
        VARIABLE                          { $$ = new vector<VarNode*>{id($1)};}
        | parameterList ',' VARIABLE        {$$ = $1;$$->push_back($3);}
        |                                   { $$ = new vector<VarNode*>();}
        ;

argumentList:
        expr                                  {$$ = new vector<ExprNode*>{$1};}
        | argumentList ',' expr               {$$ = $1;$$->push_back($3);}
        |                                     {$$ = new vector<ExprNode*>();}
        ;

declareArr:
        variable                            {$$ = declareVar($1);}
        | variable '=' expr                 {$$ = declareVar($1, $2);}
        ;

declareList:
        declareArr                          {$$ = new vector<DeclareNode*>{$1};}
        | declareList ',' declareArr        {$$ = $1;$$->push_back($3);}
        ;

subscriptionList:
        '[' expr ']'                        {$$ = new vector<ExprNode*>{$1};}
        |  subscriptionList '[' expr ']'       {$$ = $1;$$->push_back($3);}
        ;
stmt:
          ';'                                 {$$ = stmt(';', {}); }
        | expr ';'                            {$$ = stmt(EXPR, {$1};$1->inStmt();}
        | ARRAY declareList                   {$$ = stmt(ARRAY, *$2;); delete $2;}
        | FOR '(' stmt stmt stmt ')' stmt     {$$ = stmt(FOR, {$3, $4, $5, $7});}
        | WHILE '(' expr ')' stmt             {$$ = stmt(WHILE, {$3, $5});}
        | IF '(' expr ')' stmt %prec IFX      {$$ = stmt(IF, {$3, $5});}
        | IF '(' expr ')' stmt ELSE stmt      {$$ = stmt(IF, {$3, $5, $7});}
        | '{' stmtList '}'                   {$$ = stmt(STMT_LIST, *$2);delete $2;}
        | BREAK                               {$$ = stmt(BREAK, {});}
        | CONTINUE                            {$$ = stmt(CONTINUE, {});}
        | RETURN expr ';'                     {$$ = stmt(RETURN, {$2});}
        | RETURN ';'                          {$$ = stmt(RETURN, {});}  
        | DEF VARIABLE '(' parameterList ')' '{' stmtList '}'    { $$ = stmt(FUNCTION, {func($2, $4, $7)});}
        ;

variable:
        VARIABLE                { $$ = id($1);}
        | '@' VARIABLE          { $$ = id($2, true);}
        | '@' VARIABLE subscriptionList     {$$ = id($2, true, $3);}
        | variable subscrptionList { $$ = id($1, false, $2);}

stmtList:
          stmt                  {$$ = new vector<StmtNode*>{$1};}
        | stmtList stmt        {$$ = $1;$$->push_back($2);}
        ;
expr:
        | INTEGER                   {$$ = expr(INTEGER, {con($1}));}
        | STRING                    {$$ = expr(STRING, {con($1}));}
        | CHARACTER                 {$$ = expr(CHARACTER, {conChar($1)});}
        | BOOL                      {$$ = expr(BOOL, {boolean($1}));}
        | variable                  {$$ = expr(VAR, {$1});}
        | variable '=' expr         {$$ = expr('=', {$1, $3});}
        | '-' expr %prec UMINUS { $$ = expr(UMINUS, {$2});}
        | expr '+' expr         { $$ = expr('+', {$1, $3});}
        | expr '-' expr         { $$ = expr('-', {$1, $3});}
        | expr '*' expr         { $$ = expr('*', {$1, $3});}
        | expr '%' expr         { $$ = expr('%', {$1, $3});}
        | expr '/' expr         { $$ = expr('/', {$1, $3});}
        | expr '<' expr         { $$ = expr('<', {$1, $3});}
        | expr '>' expr         { $$ = expr('>', {$1, $3});}
        | expr  INC             { $$ = expr(POSINC, {$1});}
        | expr DEC             { $$ = expr(POSDEC, {$1});}
        | INC expr             { $$ = expr(PREINC, {$2});}
        | DEC expr             { $$ = expr(PREDEC, {$2});}
        | expr GE expr          { $$ = expr(GE, {$1, $3});}
        | expr LE expr          { $$ = expr(LE, {$1, $3});}
        | expr NE expr          { $$ = expr(NE, {$1, $3});}
        | expr EQ expr          { $$ = expr(EQ, {$1, $3});}
        | expr AND expr            { $$ = expr(AND, {$1, $3});}
        | expr OR expr            { $$ = expr(OR, {$1, $3});}
        | '(' expr ')'          { $$ = $2;}
        | VARIABLE '(' argumentList ')' {$$ = expr(CALL, {call($1, $3)};}
        ;
%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)




Node * con(int value) {
    return new IntNode(value);
}

ExprNode * expr(int oper, const vector<Node*>& v) {
    auto a = array<shared_prt<Node>, 2>{v[0]};
    if (v.size() == 2) {
        a[1] = v[1];
    }
    return new ExprNode(oper, a);
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

Node * stmt(int oper, const vector<Node *>& op) {
    auto v = vector<shared_ptr<Node> >();
    for (const auto i: op) {
        v.push_back(shared_ptr<Node>(i));
    }
    return new StmtNode(oper, v);
}

int getID(const string * name) {
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    return nameMap[*name];
}

Node * func(const string * name, vector<VarNode*> * parameters, vector<StmtNode*>* stmts) {
    int i = getID(name);
    if (functionSet.count(i) != 0) {
        cerr << "Redefinition of function: " << *name << endl;
        exit(-1);
    }
    auto varPar = vector<shared_ptr<VarNode> >(parameters->begin(), parameters->end());
    for (const auto & i : *parameters) {
        varPar.push_back(shared_ptr<VarNode>(i));
    }
    auto statements = vector<shared_ptr<StmtNode> >(stmts->begin(), stmts->end());
    auto p = new FunctionNode(i, varPar, statements);
    delete name;
    delete parameters;
    delete stmts;
    return p;
}



Node * id(const string * name, bool isGlobal, vector<ExprNode*>* subscriptions) {
    int i = getID(name);
    delete name;
    auto subs = vector<shared_ptr<ExprNode> >(subscriptions->begin(), subscriptions->end());
    delete subscriptions;
    return new VarNode(i, isGlobal, subs);
}

Node * call(const string * name, vector<ExprNode*> * arguments) {
    int i = getID(name);
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
