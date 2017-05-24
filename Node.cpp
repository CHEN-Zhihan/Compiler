#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include <cstdio>
using std::printf;

#include <map>
using std::map;
using std::pair;

#include <unordered_map>
using std::unordered_map;

#include <unordered_set>
using std::unordered_set;

#include "Node.h"
#include "parser.tab.h"

#include <memory>
using std::dynamic_pointer_cast;
using std::make_shared;

#include <numeric>
using std::accumulate;
using std::multiplies;

#define ADDSCOPE(node)                                                         \
    sList.push_back(++scopeCounter);                                           \
    variableTable[scopeCounter] = unordered_set<int>();                        \
    node->check(sList, functionBase);                                          \
    sList.pop_back();


static int lbl = 0;
static int scopeCounter = 1;
int GLOBAL = 0;

extern unordered_map<int, string> operatorInstruction;
extern unordered_map<int, int> functionLabel;
extern unordered_map<int, pair<int, unordered_map<int, int>>> addressTable;
map<pair<int, int>, pair<VALUE_TYPE, vector<int> > > typeTable;
extern unordered_map<int, const FunctionNode *> functionTable;
extern unordered_map<int, unordered_set<int>> variableTable;
extern vector<string> reverseLookup;

Node::Node():type(UNKNOWN){;}
Node::Node(VALUE_TYPE t):type(t){;}
VALUE_TYPE Node::getType() const {return type;}
ConNode::ConNode(VALUE_TYPE t):Node(t){;}
StrNode::StrNode(const string &v) : ConNode(STR), value(v) { ; }
void StrNode::ex(int bl, int cl, int func) const {
    cout << "\tpush\t" << value << "\n";
}

CharNode::CharNode(const string &v) :ConNode(CHAR), value(v) { ; }
void CharNode::ex(int bl, int cl, int func) const {
    cout << "\tpush\t" << value << "\n";
}

IntNode::IntNode(const int &i) : ConNode(INT), value(i) { ; }
int IntNode::getValue() const {return value;}
BoolNode::BoolNode(bool b):ConNode(BOOL), value(b){;}
void BoolNode::ex(int, int, int) const  {
    cout << "\tpush\t" << value << "\n";
}
void IntNode::ex(int, int, int) const { cout << "\tpush\t" << value << "\n"; }
void incdec(shared_ptr<VarNode> v, int blbl, int clbl, int function, bool inc) {
    v->ex(blbl, clbl, function);
    printf("\tpush\t1\n");
    if (inc) {
        printf("\tadd\n");
    } else {
        printf("\tsub\n");
    }
    v->pop(function);
}
OprNode::OprNode(int oper, const vector<shared_ptr<Node>> &op)
    : oper(oper), op(op) {
    ;
}

ExprNode::ExprNode(int oper, const vector<shared_ptr<Node>>& op):OprNode(oper, op) {;}
bool ExprNode::isKnown() const {return known;}
int ExprNode::getValue() const {return value;}
void ExprNode::ex(int blbl, int clbl, int function) const {
    if (known&&!inStatement) {
        printf("\t%d\n", value);
        return;
    }
    switch(oper) {
    case '=': {
        op[1]->ex(blbl, clbl, function);
        auto v = dynamic_pointer_cast<VarNode>(op[0]);
        v->pop(function);
        if (!inStatement) {
            v->push(function);
        }
        break;
    } case UMINUS: {
        op[0]->ex(blbl, clbl, function);
        printf("\tneg\n");
        break;
    }
    case POSINC: {
        if (!inStatement) {
            op[0]->ex(blbl, clbl, function);
        }
        incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, function,
               true);
        break;
    }    case PREINC: {
        incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, function,
               true);
        if (!inStatement) {
            op[0]->ex(blbl, clbl, function);
        }
        break;
    }
    case POSDEC: {
        if (!inStatement) {
            op[0]->ex(blbl, clbl, function);
        }
        incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, function,
               false);
        break;
    }
    case PREDEC: {
        incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, function,
               false);
        if (!inStatement) {
            op[0]->ex(blbl, clbl, function);
        }
        break;
    } default: {
        for (const auto &i : op) {
            i->ex(blbl, clbl, function);
        }
        if (operatorInstruction.count(oper) != 0) {
            cout << "\t" << operatorInstruction[oper] << "\n";
        }
    }
    }
}

void ExprNode::check(vector<int>& sList, int functionBase) const {
    for (const auto & i : op) {
        i->check(sList, functionBase);
    }
}

void ExprNode::evaluate(vector<int> & sList, int functionBase) {
    vector<int> stack;
    for (const auto & i : op) {
        auto intNode = dynamic_pointer_cast<IntNode>(i);
        auto exprNode = dynamic_pointer_cast<ExprNode>(i);
        if (intNode != nullptr) {
            stack.push_back(intNode->getValue());
        } else if (exprNode!= nullptr) {
            exprNode->evaluate(sList, functionBase);
            if (!exprNode->isKnown()) {
                known = false;
                return;
            }
            stack.push_back(exprNode->getValue());
        } else {
            known = false;
            return;
        }
    }
    int result = 0;
    switch (oper) {
        case '+': {
            result = stack[0] + stack[1];
            break;
        } case '-': {
            result = stack[0] - stack[1];
            break;
        } case '*': {
            result = stack[0] * stack[1];
            break;
        } case '/': {
            result = stack[0] / stack[1];
            break;
        } case '%': {
            result = stack[0] % stack[1];
            break;
        } case UMINUS: {
            result = -stack[0];
            break;
        } case '>': {
            result = stack[0] > stack[1];
            break;
        } case '<': {
            result = stack[0] < stack[1];
            break;
        } case GE: {
            result = stack[0] >= stack[1];
            break;
        } case LE: {
            result = stack[0] <= stack[1];
            break;
        } case EQ: {
            result = stack[0] == stack[1];
            break;
        } case NE: {
            result = stack[0] != stack[1];
            break;
        } case AND: {
            result = stack[0] && stack[1];
            break;
        } case OR: {
            result = stack[0] || stack[1];
            break;
        }
    }
    value = result;
    known = true;
}


void ValueNode::inStmt() {
    inStatement = true;
}
StmtNode::StmtNode(int oper, const vector<shared_ptr<Node>>&v):OprNode(oper, v){;}
void StmtNode::ex(int blbl, int clbl, int function) const {
    int lblx, lbly, lblz;
    switch (oper) {
    case BREAK: {
        printf("\tjmp\tL%03d\n", blbl);
        break;
    }
    case CONTINUE: {
        printf("\tjmp\tL%03d\n", clbl);
        break;
    }
    case FOR: {
        lblx = lbl++;
        lbly = lbl++;
        lblz = lbl++;
        op[0]->ex(blbl, clbl, function);
        printf("L%03d:\n", lblx);
        op[1]->ex(blbl, clbl, function);
        printf("\tj0\tL%03d\n", lbly);
        op[3]->ex(lbly, lblz, function);
        printf("L%03d:\n", lblz);
        op[2]->ex(blbl, clbl, function);
        printf("\tjmp\tL%03d\n", lblx);
        printf("L%03d:\n", lbly);
        break;
    }
    case WHILE: {
        lblx = lbl++;
        lbly = lbl++;
        printf("L%03d:\n", lblx);
        op[0]->ex(blbl, clbl, function);
        printf("\tj0\tL%03d\n", lbly);
        op[1]->ex(lbly, lblx, function);
        printf("\tjmp\tL%03d\n", lblx);
        printf("L%03d:\n", lbly);
        break;
    }
    case IF: {
        op[0]->ex(blbl, clbl, function);
        if (op.size() > 2) {
            /* if else */
            printf("\tj0\tL%03d\n", lblx = lbl++);
            op[1]->ex(blbl, clbl, function);
            printf("\tjmp\tL%03d\n", lbly = lbl++);
            printf("L%03d:\n", lblx);
            op[2]->ex(blbl, clbl, function);
            printf("L%03d:\n", lbly);
        } else {
            /* if */
            printf("\tj0\tL%03d\n", lblx = lbl++);
            op[1]->ex(blbl, clbl, function);
            printf("L%03d:\n", lblx);
        }
        break;
    }
    case RETURN: {
        if (op.size() == 0) {
            printf("\tpush\t0\n");
        } else {
            op[0]->ex(blbl, clbl, function);
        }
        printf("\tret\n");
        break;
    }
    default: {
        for (const auto & i : op) {
            i->ex(blbl, clbl, function);
        }
    }
    }
}
void StmtNode::check(vector<int> &sList, int functionBase) const {
    switch (oper) {
    case RETURN: {
        if (functionBase == GLOBAL) {
            cerr << "return occurs in the global scope" << endl;
            exit(-1);
        }
        if (op.size() != 0) {
            op[0]->check(sList, functionBase);
        }
        break;
    }
    case UMINUS: {
        op[0]->check(sList, functionBase);
        break;
    }
    case FOR: {
        sList.push_back(++scopeCounter);
        op[0]->check(sList, functionBase);
        op[1]->check(sList, functionBase);
        op[3]->check(sList, functionBase);
        op[2]->check(sList, functionBase);
        sList.pop_back();
        break;
    }
    case WHILE: {
        op[0]->check(sList, functionBase);
        ADDSCOPE(op[1]);
        break;
    }
    case IF: {
        op[0]->check(sList, functionBase);
        ADDSCOPE(op[1]);
        if (op.size() > 2) {
            ADDSCOPE(op[2]);
        }
        break;
    }
    default: {
        for (const auto & i : op) {
            i->check(sList, functionBase);
        }
    }
    }
}

IDNode::IDNode(int i, VALUE_TYPE t) : i(i), Node(t) { ; }

int IDNode::getID() const { return i; }

FunctionNode::FunctionNode(VALUE_TYPE t, int i, const list<shared_ptr<VarNode>> parameters,
                           const vector<shared_ptr<Node>> &stmts)
    : IDNode(i, t), parameters(parameters), stmts(stmts) {
    ;
}

size_t FunctionNode::getNumParameters() const { return parameters.size(); }

void FunctionNode::ex(int blbl, int clbl, int function) const { ; }

void FunctionNode::exStmt(int blbl, int clbl, int function) const {
    for (const auto & i : stmts) {
        i->ex(blbl, clbl, function);
    }
}

void FunctionNode::checkStmt(vector<int> &sList, int base) const {
    for (const auto & i : stmts) {
        i->check(sList, base);
    }
}

void FunctionNode::check(vector<int> &sList, int base) const {
    if (sList.back() != GLOBAL) {
        cerr << "Function " << reverseLookup[i]
             << " can only be defined in the global scope" << endl;
        exit(-1);
    }
    if (functionTable.count(i) != 0) {
        cerr << "Redefinition of function: " << reverseLookup[i] << endl;
        exit(-1);
    }
    variableTable[i] = unordered_set<int>();
    addressTable[i] = {0, unordered_map<int, int>()};
    int size = 0;
    functionTable[i] = this;
    unordered_set<int> duplicateSet;
    for (auto j = parameters.begin(); j != parameters.end(); ++j) {
        if (functionTable.count((*j)->getID()) != 0) {
            cerr << "Redefinition of function: " << reverseLookup[(*j)->getID()]
                 << endl;
            exit(-1);
        }
        if (duplicateSet.count((*j)->getID()) != 0) {
            cerr << "duplicate argument '" << reverseLookup[(*j)->getID()]
                 << "' in function definition" << endl;
            exit(-1);
        }
        duplicateSet.insert((*j)->getID());
        variableTable[i].insert((*j)->getID());
        typeTable[{i, (*j)->getID()}] = {type, {}};
#if DEBUG
        cerr << "add variable " << j->getID() << " to " << i << endl;
#endif
        addressTable[i].second[(*j)->getID()] =
            -3 + (size++) - parameters.size();
    }
#if DEBUG
    cerr << "adding function " << ID.i << " to " << GLOBAL << endl;
#endif
    variableTable[GLOBAL].insert(i);
}

CallNode::CallNode(int i, const list<shared_ptr<Node>> a)
    : IDNode(i), arguments(a) {
    ;
}

void CallNode::ex(int blbl, int clbl, int function) const {
    if (i < 3) {
        cout << "\t" << reverseLookup[i] << "\n";
        dynamic_cast<VarNode *>(arguments.front().get())->pop(function);
    } else if (i < 9) {
        arguments.front()->ex(blbl, clbl, function);
        cout << "\t" << reverseLookup[i] << "\n";
    } else {
        for (const auto &i : arguments) {
            i->ex(blbl, clbl, function);
        }
        printf("\tcall\tL%03d, %d\n", functionLabel[i], (int)arguments.size());
    }
}

void CallNode::check(vector<int> &sList, int functionBase) const {
    if (i < 9) {
        if (arguments.size() != 1) {
            cerr << reverseLookup[i] << " expects 1 argument, got "
                 << arguments.size() << endl;
            exit(-1);
        }
        auto first = arguments.front();
        if (i < 3) {
            auto var = dynamic_pointer_cast<VarNode>(first);
            if (!var) {
                cerr << "Invalid call to function " << reverseLookup[i] << endl;
                exit(-1);
            }
        }
        first->check(sList, functionBase);
    } else {
        if (functionTable.count(i) == 0) {
            cerr << "Call to undefined function: " << reverseLookup[i] << endl;
            exit(-1);
        }
        auto f = functionTable[i];
        if (f->getNumParameters() != arguments.size()) {
            cerr << reverseLookup[i] << " expects " << f->getNumParameters()
                 << " argument(s), got " << arguments.size() << endl;
            exit(-1);
        }
        for (const auto &i : arguments) {
            i->check(sList, functionBase);
        }
        if (functionLabel.count(i) == 0) {
            functionLabel[i] = lbl++;
            sList.push_back(i);
            f->checkStmt(sList, sList.size() - 1);
            sList.pop_back();
        }
    }
}

VarNode::VarNode(int i, bool global, const list<shared_ptr<Node>>& s) : IDNode(i), global(global), subscriptions(s) { ; }
void VarNode::setType(VALUE_TYPE t) {
    type = t;
}

void VarNode::push(int function) const {
    if (global or function == GLOBAL) {
        printf("\tpush\tsb[%d]\n", addressTable[GLOBAL].second[i]);
    } else {
        printf("\tpush\tfp[%d]\n", addressTable[function].second[i]);
    }
}

void VarNode::pop(int function) const {
    if (global or function == GLOBAL) {
        printf("\tpop\tsb[%d]\n", addressTable[GLOBAL].second[i]);
    } else {
        printf("\tpop\tfp[%d]\n", addressTable[function].second[i]);
    }
}

void VarNode::ex(int blbl, int clbl, int function) const { push(function); }

void VarNode::declare(vector<int> &sList, int functionBase) const {
    if (functionTable.count(i) != 0) {
        cerr << "Redefinition of function: " << reverseLookup[i] << endl;
        exit(-1);
    } if (global) {
        if (sList[functionBase] == GLOBAL) {
            cerr << "Refer to global variable " << reverseLookup[i]
                 << " in the global scope" << endl;
            exit(-1);
        }
        if (variableTable[GLOBAL].count(i) != 0) {
            cerr << "Redefinition of global variable " << reverseLookup[i] << endl;
            exit(-1);
        }
        variableTable[GLOBAL].insert(i);
        addressTable[GLOBAL].second[i] = addressTable[GLOBAL].first++;
    } else {
        if (variableTable[sList.back()].count(i) != 0) {
            cerr << "Redefinition of variable " << reverseLookup[i] << endl;
            exit(-1);
        }
        variableTable[sList.back()].insert(i);
        int size = accumulate(subscriptions.begin(), subscriptions.end(), 1, [&sList, functionBase, this]
            (int left, shared_ptr<Node> right) -> int {
                auto temp = dynamic_pointer_cast<ExprNode>(right);
                int value = 0;
                if (temp != nullptr) {
                    temp->evaluate(sList, functionBase);
                    value = temp->getValue();
                } else {
                    auto intNode = dynamic_pointer_cast<IntNode>(right);
                    if (intNode != nullptr) {
                        value = intNode->getValue();
                    } else {
                        cerr << "invalid size of array " << reverseLookup[i] << endl;
                        exit(-1);
                    }
                }
                if (value <= 0) {
                    cerr << "invalid size of array " << reverseLookup[i] << endl;
                    exit(-1);
                }
                return left * value;
        });
        addressTable[sList[functionBase]].second[i] = addressTable[sList[functionBase]].first;
        addressTable[sList[functionBase]].first += size;
#if DEBUG
        cerr << "add variable " << i << " to " << sList.back() << endl;
#endif
    }
}


int VarNode::getDefinitionScope(const vector<int> &sList,
                                int functionBase) const {
    for (auto j = sList.size() - 1; j != functionBase - 1; --j) {
        if (variableTable[sList[j]].count(i) != 0) {
            return sList[j];
        }
    }
    return -1;
}

void VarNode::check(vector<int> &sList, int functionBase) const {
    if (global) {
        if (variableTable[GLOBAL].count(i) == 0) {
            cerr << "Undefined reference to global variable: "
                 << reverseLookup[i] << endl;
            exit(-1);
        }
        if (sList[functionBase] == GLOBAL) {
            cerr << "Refer to a global variable: " << reverseLookup[i]
                 << " in the global scope" << endl;
            exit(-1);
        }
    } else {
        auto s = getDefinitionScope(sList, functionBase);
        if (s == -1) {
            cerr << "Undefined reference to variable " << reverseLookup[i]
                 << endl;
            exit(-1);
        }
    }
}

DeclareNode::DeclareNode(Node* variable, Node* initializer) {
    this->variable = shared_ptr<VarNode>(dynamic_cast<VarNode*>(variable));
    this->initializer = shared_ptr<Node>(initializer);
}

void DeclareNode::setType(VALUE_TYPE t) {
    type = t;
    variable->setType(t);
}

void DeclareNode::check(vector<int>& sList, int functionBase) const {
    if (initializer != nullptr) {
        initializer->check(sList, functionBase);
    }
    variable->declare(sList, functionBase);
}

void DeclareNode::ex(int blbl, int clbl, int function) const  {
    if (initializer != nullptr) {
        initializer->ex(blbl, clbl, function);
        variable->pop(function);
    }
}

ParameterNode::ParameterNode(VALUE_TYPE v, int i, int dimensions):IDNode(i, v), dimensions(dimensions) {
    ;
}

void ParameterNode::check(vector<int>&, int) const {
    ;
}

void ParameterNode::ex(int blbl, int clbl, int function) const {
    ;
}

