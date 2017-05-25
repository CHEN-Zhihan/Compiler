#include <iostream>
using std::cout;    using std::endl;
using std::cerr;

#include <cstdio>
using std::printf;

#include <map>
using std::map; using std::pair;

#include <unordered_map>
using std::unordered_map;

#include <unordered_set>
using std::unordered_set;

#include <array>
using std::array;

#include <algorithm>
using std::transform;
using std::accumulate;
using std::reverse;

#include <numeric>
using std::multiplies;

#include "Node.h"
#include "parser.tab.h"

#include <memory>
using std::dynamic_pointer_cast;    using std::make_shared;

#define ADDSCOPE(node)\
    sList.push_back(++scopeCounter);\
    variableTable[scopeCounter] = unordered_set<int>();\
    node->check(sList, functionBase);\
    sList.pop_back();

#define EVAL(oper) value = stack[0] oper stack[1]; break;

static int lbl = 0;
static int scopeCounter = 1;
int GLOBAL = 0;

const int VAR = -5;
const int CALL = -3;
extern unordered_map<int, string> operatorInstruction;
extern unordered_map<int, int> functionLabel;
extern unordered_map<int, pair<int, unordered_map<int, unordered_map<int, int> > > > addressTable;
unordered_map<int, unordered_map<int, unordered_set<int> > > assignableAddress;
map<pair<int, int>, vector<int>> sizeMap;
extern unordered_map<int, const FunctionNode* > functionTable;
extern unordered_map<int, unordered_set<int> > variableTable;
extern vector<string> reverseLookup;

void ConNode::check(vector<int>&, int) const {;}

StrNode::StrNode(const string& v):value(v) {;}
void StrNode::ex(vector<int>& sList, int, int, int) const {
    cout << "\tpush\t" << value << "\n";
}

BoolNode::BoolNode(bool b):value(b){;}
void BoolNode::ex(vector<int>& sList, int, int, int) const {
    cout << "\tpush\t" << value << "\n";
}
int BoolNode::getValue() const {return value;}
CharNode::CharNode(const string& v):value(v) {;}
void CharNode::ex(vector<int>& sList, int, int, int) const {
    cout << "\tpush\t" << value << "\n";
}
int CharNode::getValue() const {return value[1];}

IntNode::IntNode(const int& i):value(i){;}
void IntNode::ex(vector<int>& sList, int, int, int) const {
    cout << "\tpush\t" << value << "\n";
}
int IntNode::getValue() const {return value;}

void incdec(shared_ptr<VarNode> v, int blbl, int clbl, int function, bool inc) {
    v->ex(function, blbl, clbl);
    printf("\tpush\t1\n");
    if (inc) {
        printf("\tadd\n");
    } else {
        printf("\tsub\n");
    }
    v->pop(function);
}

ExprNode::ExprNode(int oper, const vector<shared_ptr<Node> > &op):oper(oper) {
    this->op = array<shared_ptr<Node>, 2>();
    this->op[0] = op[0];
    if (op.size() == 2) {
        this->op[1] = op[1];
    }
}
void ExprNode::inStmt() {
    inStatement = true;
}
void ExprNode::ex(vector<int>& sList, int function, int blbl, int clbl) const {
    unordered_set<int> uniOp{INTEGER, CHARACTER, STRING, BOOL, VAR, CALL};
    if (uniOp.count(oper) != 0) {
        op[0]->ex(sList, function, blbl, clbl);
        return;
    }
    switch (oper) {
        case '=': {
            op[1]->ex(sList, function, blbl, clbl);
            auto v = dynamic_pointer_cast<VarNode>(op[0]);
            v->pop(function);
            if (!inStatement) {
                v->push(function);
            }
            break;
        } case UMINUS: {
            op[0]->ex(sList, function, blbl, clbl);
            printf("\tneg\n");
            break;
        } case POSINC: {
            if (!inStatement) {
                op[0]->ex(sList, function, blbl, clbl);
            }
            incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, function, true);
            break;
        } case PREINC: {
            incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, function, true);
            if (!inStatement) {
                op[0]->ex(sList, function, blbl, clbl);
            }
            break;
        } case POSDEC: {
            if (!inStatement) {
                op[0]->ex(sList, function, blbl, clbl);
            }
            incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, function, false);
            break;
        } case PREDEC: {
            incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, function, false);
            if (!inStatement) {
                op[0]->ex(sList, function, blbl, clbl);
            }
            break;
        } default: {
            op[0]->ex(sList, function, blbl, clbl);
            op[1]->ex(sList, function, blbl, clbl);
            if (operatorInstruction.count(oper) != 0) {
                cout << "\t" << operatorInstruction[oper] << "\n";
            }
        }
    }
}

void ExprNode::check(vector<int>& sList, int functionBase) const {
    if (oper != '=') {
        op[0]->check(sList, functionBase);
    } else {
        op[1]->check(sList, functionBase);
        dynamic_pointer_cast<VarNode>(op[0])->assign(sList, functionBase);
        return;
    }
    unordered_set<int> uniOp{INTEGER, STRING, CHARACTER, BOOL, VAR, UMINUS, CALL, POSINC, POSDEC, PREINC, PREDEC, CALL};
    if (uniOp.count(oper) == 0) {
        op[1]->check(sList, functionBase);
    }
    if (oper == POSINC or oper == POSDEC or oper == PREINC or oper == PREDEC) {
        if (dynamic_pointer_cast<VarNode>(op[0]) == nullptr) {
            cerr << "Invalid increment/decrement operation" << endl;
            exit(-1);
        }
    }
}

void ExprNode::evaluate() {
    array<int, 2> stack;
    if (oper == STRING) {
        cerr << "Invalid operation on string" << endl;
        exit(-1);
    }
    switch (oper) {
        case INTEGER: {
            value = dynamic_pointer_cast<IntNode>(op[0])->getValue();
            known = true;
            break;
        }
        case CHARACTER: {
            value = dynamic_pointer_cast<CharNode>(op[0])->getValue();
            known = true;            
            break;
        }
        case BOOL: {
            value = dynamic_pointer_cast<BoolNode>(op[0])->getValue();
            known = true;
            break;
        }
        case VAR: {
            known = false;
            return;
        }
        default: {
            auto left = dynamic_pointer_cast<ExprNode>(op[0]);
            auto right = dynamic_pointer_cast<ExprNode>(op[1]);
            left->evaluate();
            right->evaluate();
            if (not left->known or not right->known) {
                known = false;
                return;
            }
            known = true;
            stack[0] = left->value;
            stack[1] = right->value;
            switch (oper) {
                case '+': {
                    EVAL(+);
                }
                case '-': {
                    EVAL(-);
                }
                case '*': {
                    EVAL(*);
                }
                case '/': {
                    if (stack[1] == 0) {
                        cerr << "Division by 0" << endl;
                        exit(-1);
                    }
                    EVAL(/);
                }
                case '%': {
                    if (stack[1] == 0) {
                        cerr << "Division by 0" << endl;
                        exit(-1);
                    }
                    EVAL(%);
                } case UMINUS: {
                    value = -stack[0];
                    break;
                } case '>': {
                    EVAL(>);
                } case '<': {
                    EVAL(<);
                } case GE: {
                    EVAL(>=);
                } case LE: {
                    EVAL(<=);
                } case EQ: {
                    EVAL(==);
                } case NE: {
                    EVAL(!=);
                } case AND: {
                    EVAL(&&);
                } case OR: {
                    EVAL(||);
                }
            }
        }
    }
}

int ExprNode::getValue() const {return value;}

bool ExprNode::isKnown() const {return known;}

StmtNode::StmtNode(int oper, const vector<shared_ptr<Node> >& op):oper(oper), op(op) {;}
void StmtNode::ex(vector<int>& sList, int function, int blbl, int clbl) const {
    int lblx, lbly, lblz;
    switch (oper) {
        case BREAK: {
            printf("\tjmp\tL%03d\n", blbl);
            break;
        } case CONTINUE: {
            printf("\tjmp\tL%03d\n", clbl);
            break;
        } case FOR: {
            lblx = lbl++;
            lbly = lbl++;
            lblz = lbl++;
            op[0]->ex(sList, function, blbl, clbl);
            printf("L%03d:\n", lblx);
            op[1]->ex(sList, function, blbl, clbl);
            printf("\tj0\tL%03d\n", lbly);
            op[3]->ex(sList, function, lbly, lblz);
            printf("L%03d:\n", lblz);
            op[2]->ex(sList, function, blbl, clbl);
            printf("\tjmp\tL%03d\n", lblx);
            printf("L%03d:\n", lbly);
            break;
        } case WHILE: {
            lblx = lbl++;
            lbly = lbl++;
            printf("L%03d:\n", lblx);
            op[0]->ex(sList, function, blbl, clbl);
            printf("\tj0\tL%03d\n", lbly);
            op[1]->ex(sList, function, lbly, lblx);
            printf("\tjmp\tL%03d\n", lblx);
            printf("L%03d:\n", lbly);
            break;
        } case IF: {
            op[0]->ex(sList, function, blbl, clbl);
            if (op.size() > 2) {
                /* if else */
                printf("\tj0\tL%03d\n", lblx = lbl++);
                op[1]->ex(sList, function, blbl, clbl);
                printf("\tjmp\tL%03d\n", lbly = lbl++);
                printf("L%03d:\n", lblx);
                op[2]->ex(sList, function, blbl, clbl);
                printf("L%03d:\n", lbly);
            } else {
                /* if */
                printf("\tj0\tL%03d\n", lblx = lbl++);
                op[1]->ex(sList, function, blbl, clbl);
                printf("L%03d:\n", lblx);
            }
            break;
        } case RETURN: {
            if (op.size() == 0) {
                printf("\tpush\t0\n");
            } else {
                op[0]->ex(sList, function, blbl, clbl);
            }
            printf("\tret\n");
            break;
        } default: {
            for (const auto & i : op) {
                i->ex(sList, function, blbl, clbl);
            }
        }
    }
}
void StmtNode::check(vector<int>& sList, int functionBase) const {
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
        } case FOR: {
            sList.push_back(++scopeCounter);
            op[0]->check(sList, functionBase);
            op[1]->check(sList, functionBase);
            op[3]->check(sList, functionBase);
            op[2]->check(sList, functionBase);
            sList.pop_back();
            break;
        } case WHILE: {
            op[0]->check(sList, functionBase);
            ADDSCOPE(op[1]);
            break;
        } case IF: {
            op[0]->check(sList, functionBase);
            ADDSCOPE(op[1]);
            if (op.size() > 2) {
                ADDSCOPE(op[2]);
            }
            break;
        } default: {
            for (const auto & i: op) {
                i->check(sList, functionBase);
            }
        }
    }
}


IDNode::IDNode(int i):i(i){;}

int IDNode::getID() const {
    return i;
}

FunctionNode::FunctionNode(int i,
                           const vector<shared_ptr<VarNode> >& parameters,
                           const vector<shared_ptr<Node> >& stmts): IDNode(i), parameters(parameters), stmts(stmts){;}

size_t FunctionNode::getNumParameters() const {
    return parameters.size();
}

void FunctionNode::ex(vector<int>& sList, int function, int blbl, int clbl) const {
    ;
}

void FunctionNode::exStmt(vector<int>& sList, int function, int blbl, int clbl) const {
    for (const auto & i : stmts) {
        i->ex(sList, function, blbl, clbl);
    }
}

void FunctionNode::checkStmt(vector<int>& sList, int base) const {
    for (const auto & i : stmts) {
        i->check(sList, base);
    }
}

void FunctionNode::check(vector<int>& sList, int base) const {
    if (sList.back() != GLOBAL) {
        cerr << "Function " << reverseLookup[i] << " can only be defined in the global scope" << endl;
        exit(-1);
    }
    if (functionTable.count(i) != 0) {
        cerr << "Redefinition of function: " << reverseLookup[i] << endl;
        exit(-1);
    }
    variableTable[i] = unordered_set<int>();
    addressTable[i] = {0, unordered_map<int, unordered_map<int, int>>()};
    int size = 0;
    functionTable[i] = this;
    unordered_set<int> duplicateSet;
    for (auto j = parameters.begin(); j != parameters.end(); ++j) {
        if (functionTable.count((*j)->getID()) != 0) {
            cerr << "Redefinition of function: " << reverseLookup[(*j)->getID()] << endl;
            exit(-1);
        }
        if (duplicateSet.count((*j)->getID()) != 0) {
            cerr << "duplicate argument '" << reverseLookup[(*j)->getID()] << "' in function definition" << endl;
            exit(-1);
        }
        duplicateSet.insert((*j)->getID());
        variableTable[i].insert((*j)->getID());
        #if DEBUG
            cerr << "add variable " << j->getID() << " to " << i << endl;
        #endif
        addressTable[i].second[][(*j)->getID()] = - 3 + (size++) - parameters.size();
    }
    #if DEBUG
        cerr << "adding function " << ID.i << " to " << GLOBAL << endl;
    #endif
    variableTable[GLOBAL].insert(i);
}

CallNode::CallNode(int i, const vector<shared_ptr<ExprNode> >& a):IDNode(i), arguments(a) {;}

void CallNode::ex(vector<int>& sList, int function, int blbl, int clbl) const {
    if (i < 3) {
        cout << "\t" << reverseLookup[i] << "\n";
    } else if (i < 9) {
        arguments.front()->ex(sList, function, blbl, clbl);
        cout << "\t" << reverseLookup[i] << "\n";
    } else {
        for (const auto& i: arguments) {
            i->ex(sList, function, blbl, clbl);
        }
        printf("\tcall\tL%03d, %d\n", functionLabel[i], (int)arguments.size());
    }
}

void CallNode::check(vector<int> & sList, int functionBase) const {
    if (i < 9) {
        if (arguments.size() != 1 and i >= 3) {
            cerr << reverseLookup[i] << " expects 1 argument, got " << arguments.size() << endl;
            exit(-1); 
        }
        if (arguments.size() != 0 and i < 3) {
            cerr << reverseLookup[i] << " expects 0 argument, got " << arguments.size() << endl;
            exit(-1); 
        }
        if (i >= 3) {
            auto first = arguments.front();
            first->check(sList, functionBase);
        }
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
        for (const auto & i: arguments) {
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

VarNode::VarNode(int i, bool global, const vector<shared_ptr<ExprNode>>& s):IDNode(i), global(global), subscriptions(s) {;}

void VarNode::push(vector<int>& sList, int function) const {
    if (subscriptions.size() == 0) {
        if (global or function == GLOBAL) {
            printf("\tpush\tsb[%d]\n", addressTable[GLOBAL].second[GLOBAL][i]);
        } else {
            printf("\tpush\tfp[%d]\n", addressTable[function].second[function][i]);
        }
        return;
    }
    getOffSet(sList, function, -1, -1);
    if (global or function == GLOBAL) {
        printf("\tpush\tsb[sb]\n");
    } else {
        printf("\tpush\tfp[sb]\n");
    }
}

void VarNode::pop(int function) const {
    if (subscriptions.size() == 0) {
        if (global or function == GLOBAL) {
            printf("\tpop\tsb[%d]\n", addressTable[GLOBAL].second[GLOBAL][i]);
        } else {
            printf("\tpop\tfp[%d]\n", addressTable[function].second[][i]);
        }
        return;
    }
    getOffSet(function, -1, -1);
    if (global or function == GLOBAL) {
        printf("\tpop\tsb[sb]\n");
    } else {
        printf("\tpop\tfp[sb]\n");
    }
}

void VarNode::ex(vector<int>& sList, int function, int blbl, int clbl) const {
    push(function);
}
void VarNode::getOffSet(vector<int>& sList, int function, int blbl, int clbl) const {
    auto dimension = sizeMap[{global?GLOBAL:function, i}];
    auto size = vector<int>();
    int accumulate = 1;
    for (auto j = dimension.size() -1 ; j != 0; --j) {
        accumulate *= dimension[j];
        size.push_back(accumulate);
    }
    reverse(size.begin(), size.end());
    vector<int> values;
    vector<int> unknowns;
    for (int j = 0; j != subscriptions.size();++j) {
        if (subscriptions[j]->isKnown()) {
            auto value = subscriptions[j]->getValue();
            if (value < 0) {
                cerr << "invalid index of array " << i << endl;
                exit(-1);
            }
            values.push_back(value);
        } else {
            values.push_back(-1);
        }
    }
    for (int i = 0; i != values.size() - 1; ++i) {
        if (values[i] != -1) {
            printf("\tpush\t%d\n", size[i] * values[i]);
        } else {
            subscriptions[i]->ex(sList, function, blbl, clbl);
            printf("\tpush\t%d\n", size[i]);
            printf("\tmul\n");
        }
    }
    if (not subscriptions.back()->isKnown()) {
        subscriptions.back()->ex(sList, function, blbl, clbl);
    } else {
        printf("\tpush\t%d\n", subscriptions.back()->getValue());
    }
    for (int i = 0; i != subscriptions.size() - 1 ; ++i) {
        printf("\tadd\n");
    }
    printf("\tpush\t%d\n", addressTable[global?GLOBAL:function].second[getDefinitionScope(sList, functionBase)][i]);
    printf("\tadd\n");
    printf("\tpop\tsb\n");
}

vector<int> VarNode::getDimension() const {
    if (subscriptions.size() == 0) {
        return {};
    }
    auto result = vector<int>(subscriptions.size());
    transform(subscriptions.begin(), subscriptions.end(), result.begin(), [this] (shared_ptr<ExprNode> node) -> int {
        node->evaluate();
        if (!node->isKnown()) {
            cerr << "Invalid size of array " << reverseLookup[i] << endl;
            exit(-1);
        }
        return node->getValue();
    });
    return result;
}

void VarNode::declare(vector<int> &sList, int functionBase) const {
    if (functionTable.count(i) != 0) {
        cerr << "Redefinition of function: " << reverseLookup[i] << endl;
        exit(-1);
    }
    auto dimension = getDimension();
    int size = accumulate(dimension.begin(), dimension.end(), 1, multiplies<int>());
    if (size <= 0) {
        cerr << "Invalid length of array " << reverseLookup[i] << endl;
        exit(-1);
    }
    if (global) {
        if (sList[functionBase] == GLOBAL) {
            cerr << "Refer to global variable " << reverseLookup[i] << " in the global scope" << endl;
            exit(-1);
        }
        if (variableTable[GLOBAL].count(i) != 0) {
            cerr << "Redeclaration of variable " << reverseLookup[i] << endl;
            exit(-1);
        }
        addressTable[GLOBAL].second[GLOBAL][i] = addressTable[GLOBAL].first;
        addressTable[GLOBAL].first += size;
        variableTable[GLOBAL].insert(i);
        sizeMap[{GLOBAL, i}] = dimension;
    } else {
        int s = getDefinitionScope(sList, functionBase);
        if (s != -1) {
            cerr << "Redeclaration of variable " << reverseLookup[i] << endl;
            exit(-1);
        }
        variableTable[sList.back()].insert(i);
        #if DEBUG
            cerr << "add variable " << i << " to " << sList.back() << endl;
        #endif
        addressTable[sList[functionBase]].second[sList.back()][i] = addressTable[sList[functionBase]].first;
        addressTable[sList[functionBase]].first += size;
        sizeMap[{sList.back(), i}] = dimension;
    }
}

void VarNode::assign(vector<int> & sList, int functionBase) const {
    if (functionTable.count(i) != 0) {
        cerr << "Redefinition of function: " << reverseLookup[i] << endl;
        exit(-1);
    }
    for (auto & i : subscriptions) {
        i->check(sList, functionBase);
        i->evaluate();
    }
    if (global) {
        if (sList[functionBase] == GLOBAL) {
            cerr << "Refer to global variable " << reverseLookup[i] << " in the global scope" << endl;
            exit(-1);
        }
        if (variableTable[GLOBAL].count(i) != 0 and sizeMap[{GLOBAL, i}].size() != subscriptions.size()) {
            cerr << "Invalid assignment to array " << reverseLookup[i] << endl;
            exit(-1);
        }
        if (variableTable[GLOBAL].count(i) == 0) {
            variableTable[GLOBAL].insert(i);
            addressTable[GLOBAL].second[GLOBAL][i] = addressTable[GLOBAL].first++;
            sizeMap[{GLOBAL, i}] = vector<int>();
        }
    } else {
        int s = getDefinitionScope(sList, functionBase);
        if (s != -1 and sizeMap[{s, i}].size() != subscriptions.size()) {
            cerr << "Invalid assignment to array " << reverseLookup[i] << endl;
            exit(-1);
        }
        if (s == -1) {
            addressTable[sList[functionBase]].second[sList.back()][i] = addressTable[sList[functionBase]].first++;
            variableTable[sList.back()].insert(i);
            #if DEBUG
                cerr << "add variable " << i << " to " << sList.back() << endl;
            #endif
            sizeMap[{sList.back(), i}] = vector<int>();
        }
    }
}

int VarNode::getDefinitionScope(const vector<int>& sList, int functionBase) const {
    for (auto j = sList.size() - 1; j != functionBase - 1; --j) {
        if (variableTable[sList[j]].count(i) != 0) {
            return sList[j];
        }
    }
    return -1;
}

void VarNode::check(vector<int>& sList, int functionBase) const {
    if (global) {
        if (variableTable[GLOBAL].count(i) == 0) {
            cerr << "Undefined reference to global variable: " << reverseLookup[i] << endl;
            exit(-1);
        }
        if (sList[functionBase] == GLOBAL) {
            cerr << "Refer to a global variable: " << reverseLookup[i] << " in the global scope" << endl;
            exit(-1);
        }
    } else {
        auto s = getDefinitionScope(sList, functionBase);
        if (s == -1) {
            cerr << "Undefined reference to variable " << reverseLookup[i] << endl;
            exit(-1);
        }
    }
    for (auto & i : subscriptions) {
        i->check(sList, functionBase);
        i->evaluate();
    }
}

DeclareNode::DeclareNode(const shared_ptr<VarNode>& v, const shared_ptr<ExprNode> e):variable(v), initializer(e) {;}
void DeclareNode::ex(vector<int>& sList, int function, int blbl, int clbl) const {
    if (initializer != nullptr) {
        initializer->ex(sList, function, blbl, clbl);
    }
}

void DeclareNode::check(vector<int>& sList, int functionBase) const {
    if (initializer != nullptr) {
        initializer->check(sList, functionBase);
    }
    variable->declare(sList, functionBase);
}