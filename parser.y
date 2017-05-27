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
Node* stmt(int, const vector<Node*>*);
Node* con(int value);
Node* con(const string * value);
Node* conChar(const string * value);
Node* boolean(bool);
VarNode* id(const string *, bool isGlobal=false, vector<ExprNode*>* subscriptions=nullptr);
Node* call(const string *, vector<ExprNode*>*);
Node* func(const string *, vector<VarNode*>*, vector<Node*>*);
void run(shared_ptr<Node> p);
int yylex(void);
void yyerror(char *s);
static unordered_map<string, int> nameMap;
set<int> functionSet;
set<int> callSet;
vector<string> reverseLookup;
static int nameCounter;
const int STMT_LIST = -1;
const int EXPR = -2;
const int CALL = -3;
const int FUNCTION = -4;
const int VAR = -5;
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
    DeclareNode *declareNode;
    vector<VarNode*>* varList;
    vector<ExprNode*>* exprList;
    vector<Node*>* nodeList;
};

%token <iValue> INTEGER
%token <cValue> CHARACTER
%token <sValue> STRING
%token <bValue> BOOL
%token <variable> VARIABLE
%token FOR WHILE IF PRINT READ BREAK CONTINUE END RETURN DEF ARRAY POSINC POSDEC PREINC PREDEC
%nonassoc IFX
%nonassoc ELSE

%left AND OR

%left GE LE EQ NE '>' '<'
%right '='
%left '+' '-' 
%left '*' '/' '%'
%right INC DEC
%nonassoc UMINUS NOT

%type <nPtr> stmt variable

%type <exprNode> expr
%type <varList> parameterList parList
%type <exprList> subscriptionList argumentList argList
%type <nodeList> stmtList declareList
%type <declareNode> declareArr
%type <iValue>dimensions dim
%%



program:
        stmtList END                          {
                                                    run(shared_ptr<Node>(stmt(STMT_LIST, $1)));
                                                    exit(0);
                                                }
        ;
dim:
        '[' ']'                             {$$ = 1;}
        |dim '[' ']'                        {$$ = $1 + 1;}
        ;
dimensions:
                                            {$$ = 0;}
        | dim                               {$$ = $1;}
        ;

parList:
                                           {$$ = new vector<VarNode*>();}
        | parameterList                     {$$ = $1;}
        ;
parameterList:
        VARIABLE dimensions                         { $$ = new vector<VarNode*>{id($1, false, new vector<ExprNode*>($2))};}
        | parameterList ',' VARIABLE dimensions       {$$ = $1;$$->push_back(id($3, false, new vector<ExprNode*>($4)));}
        ;

argList:
                                                {$$ = new vector<ExprNode*>();}
        | argumentList                          {$$ = $1;}
        ;

argumentList:
        expr                                  {$$ = new vector<ExprNode*>{$1};}
        | argumentList ',' expr               {$$ = $1;$$->push_back($3);}
        ;

declareArr:
        variable                            {$$ = new DeclareNode(dynamic_pointer_cast<VarNode>(shared_ptr<Node>($1)));}
        | variable '=' expr                 {$$ = new DeclareNode(dynamic_pointer_cast<VarNode>(shared_ptr<Node>($1)), shared_ptr<ExprNode>($3));}
        ;

declareList:
        declareArr                          {$$ = new vector<Node*>{$1};}
        | declareList ',' declareArr        {$$ = $1;$$->push_back($3);}
        ;

subscriptionList:
        '[' expr ']'                        {$$ = new vector<ExprNode*>{$2};}
        |  subscriptionList '[' expr ']'       {$$ = $1;$$->push_back($3);}
        ;
stmt:
          ';'                                 {$$ = stmt(';', vector<Node*>()); }
        | expr ';'                            {$$ = stmt(EXPR, {$1});$1->inStmt();}
        | ARRAY declareList ';'                  {$$ = stmt(ARRAY, $2);}
        | FOR '(' stmt stmt stmt ')' stmt     {$$ = stmt(FOR, {$3, $4, $5, $7});}
        | WHILE '(' expr ')' stmt             {$$ = stmt(WHILE, {$3, $5});}
        | IF '(' expr ')' stmt %prec IFX      {$$ = stmt(IF, {$3, $5});}
        | IF '(' expr ')' stmt ELSE stmt      {$$ = stmt(IF, {$3, $5, $7});}
        | '{' stmtList '}'                   {$$ = stmt(STMT_LIST, $2);}
        | BREAK ';'                              {$$ = stmt(BREAK, vector<Node*>());}
        | CONTINUE ';'                           {$$ = stmt(CONTINUE, vector<Node*>());}
        | RETURN expr ';'                     {$$ = stmt(RETURN, {$2});}
        | RETURN ';'                          {$$ = stmt(RETURN, vector<Node*>());}
        | DEF VARIABLE '(' parList ')' '{' stmtList '}'    { $$ = stmt(FUNCTION, {func($2, $4, $7)});}
        ;

variable:
        VARIABLE                { $$ = id($1);}
        | '@' VARIABLE          { $$ = id($2, true);}
        | '@' VARIABLE subscriptionList     {$$ = id($2, true, $3);}
        | VARIABLE subscriptionList { $$ = id($1, false, $2);}
        ;
stmtList:
          stmt                  {$$ = new vector<Node*>();$$->push_back($1);}
        | stmtList stmt        {$$ = $1;$$->push_back($2);}
        ;
expr:
        INTEGER                   {$$ = expr(INTEGER, {con($1)});}
        | STRING                    {$$ = expr(STRING, {con($1)});}
        | CHARACTER                 {$$ = expr(CHARACTER, {conChar($1)});}
        | BOOL                      {$$ = expr(BOOL, {boolean($1)});}
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
        | variable INC             { $$ = expr(POSINC, {$1});}
        | variable DEC             { $$ = expr(POSDEC, {$1});}
        | INC variable             { $$ = expr(PREINC, {$2});}
        | DEC variable             { $$ = expr(PREDEC, {$2});}
        | expr GE expr          {$$ = expr(GE, {$1, $3});}
        | expr LE expr          {$$ = expr(LE, {$1, $3});}
        | expr NE expr          {$$ = expr(NE, {$1, $3});}
        | expr EQ expr          {$$ = expr(EQ, {$1, $3});}
        | expr AND expr            {$$ = expr(AND, {$1, $3});}
        | expr OR expr            {$$ = expr(OR, {$1, $3});}
        | NOT expr            {$$ = expr(NOT, {$2});}
        | '(' expr ')'          {$$ = $2;}
        | VARIABLE '(' argList ')' {$$ = expr(CALL, {call($1, $3)});}
        ;
%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)




Node * con(int value) {
    return new IntNode(value);
}

ExprNode * expr(int oper, const vector<Node*>& v) {
    auto vs = vector<shared_ptr<Node>>(v.begin(), v.end());
    return new ExprNode(oper, vs);
}

Node* boolean(bool b) {
    return new BoolNode(b);
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

Node* stmt(int oper, const vector<Node*>* v) {
    auto sList = vector<shared_ptr<Node> >(v->begin(), v->end());
    delete v;
    return new StmtNode(oper, sList);
}

Node * stmt(int oper, const vector<Node *>& op) {
    auto v = vector<shared_ptr<Node> >(op.begin(), op.end());
    return new StmtNode(oper, v);
}

int getID(const string * name) {
    if (nameMap.count(*name) == 0) {
        nameMap[*name] = nameCounter++;
        reverseLookup.push_back(*name);
    }
    return nameMap[*name];
}

Node * func(const string * name, vector<VarNode*> * parameters, vector<Node*>* stmts) {
    int i = getID(name);
    if (functionSet.count(i) != 0) {
        cerr << "Redefinition of function: " << *name << endl;
        exit(-1);
    }
    auto varPar = vector<shared_ptr<VarNode> >(parameters->begin(), parameters->end());
    auto statements = vector<shared_ptr<Node> >(stmts->begin(), stmts->end());
    auto p = new FunctionNode(i, varPar, statements);
    delete name;
    delete parameters;
    delete stmts;
    return p;
}



VarNode * id(const string * name, bool isGlobal, vector<ExprNode*>* subscriptions) {
    int i = getID(name);
    delete name;
    auto subs = vector<shared_ptr<ExprNode> >();
    if (subscriptions != nullptr) {
        subs = vector<shared_ptr<ExprNode> >(subscriptions->begin(), subscriptions->end());
        delete subscriptions;
    }
    return new VarNode(i, isGlobal, subs);
}

Node * call(const string * name, vector<ExprNode*> * arguments) {
    int i = getID(name);
    callSet.insert(i);
    auto argList = vector<shared_ptr<ExprNode>>(arguments->begin(), arguments->end());
    auto p = new CallNode(i, argList);
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
