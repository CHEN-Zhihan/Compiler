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

#include <set>
using std::set;

#include <array>
using std::array;

#include <algorithm>
using std::transform;
using std::reverse;

#include <numeric>
using std::multiplies;
using std::accumulate;


#include "Node.h"
#include "parser.tab.h"

#include <memory>
using std::dynamic_pointer_cast;    using std::make_shared;

using scope = int;
using address = int;
using ID = int;

#define ADDSCOPE(node)\
    scopes.push_back(--scopeCounter);\
    sList.push_back(scopes.back());\
    variableTable[scopeCounter] = unordered_set<int>();\
    addressTable[functionID].second[scopeCounter] = unordered_map<int, int>();\
    node->check(sList, functionID);\
    releaseSpace(functionID, sList.back());\
    sList.pop_back();\

#define EVAL(oper) value = stack[0] oper stack[1]; break;

static int lbl = 0;
extern int scopeCounter;
int GLOBAL = 0;
constexpr int NOT_FOUND = 2000;

const int VAR = -5;
const int CALL = -3;
extern unordered_map<int, string> operatorInstruction;
extern unordered_map<int, int> functionLabel;
extern unordered_map<int, pair<int, unordered_map<scope, unordered_map<ID, address> > > > addressTable;
extern unordered_map<int, set<pair<address, int> > > reusableAddress;
map<pair<int, int>, vector<int>> sizeMap;
extern unordered_map<int, FunctionNode* > functionTable;
extern unordered_map<int, unordered_set<int> > variableTable;
extern vector<string> reverseLookup;

int findSpace(int functionID, int targetSize) {
    for (const auto & i : reusableAddress[functionID]) {
        if (i.second == targetSize) {
            #if DEBUG
                cerr << "found a space at " << i.first << " of size " << targetSize << endl;
            #endif
            reusableAddress[functionID].erase(i);
            return i.first;
        } else if (i.second > targetSize) {
            #if DEBUG
                cerr << "found a space at " << i.first << " of size " << i.second << " " << i.second - targetSize << " left" << endl;
            #endif
            reusableAddress[functionID].erase(i);
            reusableAddress[functionID].insert({i.first + targetSize, i.second - targetSize});
            return i.first;
        }
    }
    return -1;
}

void releaseSpace(int functionID, int scopeID) {
    for (const auto & i : addressTable[functionID].second[scopeID]) {
        auto dimension = sizeMap[{scopeID, i.first}];
        auto size = accumulate(dimension.begin(), dimension.end(), 1, multiplies<int>());
        auto p = pair<int, int>{i.second, size};
        reusableAddress[functionID].insert(p);
        #if DEBUG
            cerr << "Returning space at " << i.second << " of size " << size << endl; 
        #endif
    }
}

void ConNode::check(vector<int>&, int) {;}

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

void incdec(shared_ptr<VarNode> v, int blbl, int clbl, vector<int>& sList, int function, bool inc) {
    v->ex(sList, function, blbl, clbl);
    printf("\tpush\t1\n");
    if (inc) {
        printf("\tadd\n");
    } else {
        printf("\tsub\n");
    }
    v->pop(sList, function);
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
void ExprNode::ex(vector<int>& sList, int functionID, int blbl, int clbl) const {
    unordered_set<int> uniOp{INTEGER, CHARACTER, STRING, BOOL, VAR, CALL};
    if (uniOp.count(oper) != 0) {
        op[0]->ex(sList, functionID, blbl, clbl);
        return;
    }
    switch (oper) {
        case '=': {
            op[1]->ex(sList, functionID, blbl, clbl);
            auto v = dynamic_pointer_cast<VarNode>(op[0]);
            v->pop(sList, functionID);
            if (!inStatement) {
                v->push(sList, functionID);
            }
            break;
        } case NOT: {
            printf("\tpush\t1\n");
            op[0]->ex(sList, functionID, blbl, clbl);
            printf("\tsub\n");
            break;

        } case UMINUS: {
            op[0]->ex(sList, functionID, blbl, clbl);
            printf("\tneg\n");
            break;
        } case POSINC: {
            if (!inStatement) {
                op[0]->ex(sList, functionID, blbl, clbl);
            }
            incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, sList, functionID, true);
            break;
        } case PREINC: {
            incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, sList, functionID, true);
            if (!inStatement) {
                op[0]->ex(sList, functionID, blbl, clbl);
            }
            break;
        } case POSDEC: {
            if (!inStatement) {
                op[0]->ex(sList, functionID, blbl, clbl);
            }
            incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, sList, functionID, false);
            break;
        } case PREDEC: {
            incdec(dynamic_pointer_cast<VarNode>(op[0]), blbl, clbl, sList, functionID, false);
            if (!inStatement) {
                op[0]->ex(sList, functionID, blbl, clbl);
            }
            break;
        } default: {
            op[0]->ex(sList, functionID, blbl, clbl);
            op[1]->ex(sList, functionID, blbl, clbl);
            if (operatorInstruction.count(oper) != 0) {
                cout << "\t" << operatorInstruction[oper] << "\n";
            }
        }
    }
}

void ExprNode::checkArray(vector<int>& sList, int functionID, const shared_ptr<Node>& t) const {
    auto e = dynamic_pointer_cast<VarNode>(t);
    if (e == nullptr) {
        return;
    }
    if (!e->matched) {
        if (e != nullptr and e->getDimensions() != sizeMap[{e->getDefinitionScope(sList, functionID), e->getID()}].size()) {
            cerr << "Invalid operation on array " << reverseLookup[e->getID()] << endl;
            exit(-1);
        }
    }

}


void ExprNode::check(vector<int>& sList, int functionID) {
    if (oper != '=') {
        op[0]->check(sList, functionID);
    } else {
        op[1]->check(sList, functionID);
        dynamic_pointer_cast<VarNode>(op[0])->assign(sList, functionID);
        return;
    }
    checkArray(sList, functionID, op[0]);
    unordered_set<int> uniOp{NOT, INTEGER, STRING, CHARACTER, BOOL, VAR, UMINUS, CALL, POSINC, POSDEC, PREINC, PREDEC, CALL};
    if (uniOp.count(oper) == 0) {
        checkArray(sList, functionID, op[1]);
        op[1]->check(sList, functionID);
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
void StmtNode::ex(vector<int>& sList, int functionID, int blbl, int clbl) const {
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
            sList.push_back(scopes.back());
            op[0]->ex(sList, functionID, blbl, clbl);
            printf("L%03d:\n", lblx);
            op[1]->ex(sList, functionID, blbl, clbl);
            printf("\tj0\tL%03d\n", lbly);
            op[3]->ex(sList, functionID, lbly, lblz);
            printf("L%03d:\n", lblz);
            op[2]->ex(sList, functionID, blbl, clbl);
            sList.pop_back();
            printf("\tjmp\tL%03d\n", lblx);
            printf("L%03d:\n", lbly);
            break;
        } case WHILE: {
            lblx = lbl++;
            lbly = lbl++;
            printf("L%03d:\n", lblx);
            op[0]->ex(sList, functionID, blbl, clbl);
            printf("\tj0\tL%03d\n", lbly);
            sList.push_back(scopes.back());
            op[1]->ex(sList, functionID, lbly, lblx);
            sList.pop_back();
            printf("\tjmp\tL%03d\n", lblx);
            printf("L%03d:\n", lbly);
            break;
        } case IF: {
            op[0]->ex(sList, functionID, blbl, clbl);
            if (op.size() > 2) {
                /* if else */
                printf("\tj0\tL%03d\n", lblx = lbl++);
                sList.push_back(scopes[0]); // if has 2 scopes;
                op[1]->ex(sList, functionID, blbl, clbl);
                sList.pop_back();
                printf("\tjmp\tL%03d\n", lbly = lbl++);
                printf("L%03d:\n", lblx);
                sList.push_back(scopes[1]);
                op[2]->ex(sList, functionID, blbl, clbl);
                sList.pop_back();
                printf("L%03d:\n", lbly);
            } else {
                /* if */
                printf("\tj0\tL%03d\n", lblx = lbl++);
                sList.push_back(scopes.back());
                op[1]->ex(sList, functionID, blbl, clbl);
                sList.pop_back();
                printf("L%03d:\n", lblx);
            }
            break;
        } case RETURN: {
            if (op.size() == 0) {
                printf("\tpush\t0\n");
            } else {
                op[0]->ex(sList, functionID, blbl, clbl);
            }
            printf("\tret\n");
            break;
        } default: {
            for (const auto & i : op) {
                i->ex(sList, functionID, blbl, clbl);
            }
        }
    }
}
void StmtNode::check(vector<int>& sList, int functionID) {
    switch (oper) {
        case RETURN: {
            if (functionID == GLOBAL) {
                cerr << "return occurs in the global scope" << endl;
                exit(-1);
            }
            if (op.size() != 0) {
                op[0]->check(sList, functionID);
            }
            break;
        } case FOR: {
            scopes.push_back(--scopeCounter);
            sList.push_back(scopes[0]);
            variableTable[scopeCounter] = unordered_set<int>();
            addressTable[functionID].second[scopeCounter] = unordered_map<int, int>();
            op[0]->check(sList, functionID);
            op[1]->check(sList, functionID);
            op[3]->check(sList, functionID);
            op[2]->check(sList, functionID);
            releaseSpace(functionID, sList.back());
            sList.pop_back();
            break;
        } case WHILE: {
            op[0]->check(sList, functionID);
            ADDSCOPE(op[1]);
            break;
        } case IF: {
            op[0]->check(sList, functionID);
            ADDSCOPE(op[1]);
            if (op.size() > 2) {
                ADDSCOPE(op[2]);
            }
            break;
        } default: {
            for (const auto & i: op) {
                i->check(sList, functionID);
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

void FunctionNode::ex(vector<int>& sList, int functionID, int blbl, int clbl) const {
    ;
}

void FunctionNode::exStmt(vector<int>& sList, int functionID, int blbl, int clbl) const {
    for (const auto & i : stmts) {
        i->ex(sList, functionID, blbl, clbl);
    }
}

void FunctionNode::checkStmt(vector<int>& sList, int base) {
    scope = sList.back();
    for (const auto & i : stmts) {
        i->check(sList, base);
    }
}

void FunctionNode::check(vector<int>& sList, int functionID) {
    if (sList.back() != GLOBAL) {
        cerr << "Function " << reverseLookup[i] << " can only be defined in the global scope" << endl;
        exit(-1);
    }
    if (functionTable.count(i) != 0) {
        cerr << "Redefinition of function: " << reverseLookup[i] << endl;
        exit(-1);
    }
    variableTable[i] = unordered_set<int>();
    addressTable[i] ={0, unordered_map<int, unordered_map<ID, address> >()};
    int size = 0;
    functionTable[i] = this;
    unordered_set<int> duplicateSet;
    for (auto j = parameters.begin(); j != parameters.end(); ++j) {
        if (functionTable.count((*j)->getID()) != 0) {
            cerr << "Redefinition of function: " << reverseLookup[(*j)->getID()] << endl;
            exit(-1);
        }
        if (duplicateSet.count((*j)->getID()) != 0) {
            cerr << "duplicate parameter '" << reverseLookup[(*j)->getID()] << "' in function definition" << endl;
            exit(-1);
        }
        duplicateSet.insert((*j)->getID());
        variableTable[i].insert((*j)->getID());
        #if DEBUG
            cerr << "add variable " << reverseLookup[(*j)->getID()] << " to " << i << endl;
            cerr << "add variable of size " << (*j)->getDimensions() << " to " << i << " " << (*j)->getID() << endl;
        #endif
        addressTable[i].second[i][(*j)->getID()] = - 3 + (size++) - parameters.size();
        sizeMap[{i, (*j)->getID()}] = vector<int>((*j)->getDimensions());
    }
    #if DEBUG
        cerr << "adding function " << reverseLookup[this->i] << " to " << GLOBAL << endl;
    #endif
    variableTable[GLOBAL].insert(i);
}

void FunctionNode::match(const vector<shared_ptr<ExprNode> >& arguments, const vector<int>& sList, int base) const {
    for (int i = 0; i != arguments.size(); ++i) {
        auto parameter = parameters[i];
        auto argument = arguments[i];
        if (parameter->getDimensions() != 0) {
            if (argument->oper != VAR) {
                cerr << "Invalid " << i << " th argument for function " << reverseLookup[this->i] << endl;
                exit(-1);
            }
            auto v = dynamic_pointer_cast<VarNode>(argument->op[0]);
            auto dimension = sizeMap[{v->getDefinitionScope(sList, base), v->getID()}];
            if (parameter->getDimensions() != dimension.size()) {
                #if DEBUG
                    cerr << "expect " << parameter->getDimensions() <<
                    " got " << dimension.size() << " for function " << reverseLookup[this->i] << endl;
                #endif
                cerr << "Invalid " << i << " th argument for function " << reverseLookup[this->i] << endl;
                exit(-1);
            }
            v->match();
            sizeMap[{this->i, parameter->getID()}] = dimension;
        }
    }
}

CallNode::CallNode(int i, const vector<shared_ptr<ExprNode> >& a):IDNode(i), arguments(a) {;}

void CallNode::ex(vector<int>& sList, int function, int blbl, int clbl) const {
    if (i < 3) {
        cout << "\t" << reverseLookup[i] << "\n";
    } else if (i < 9) {
        arguments.back()->ex(sList, function, blbl, clbl);
        cout << "\t" << reverseLookup[i] << "\n";
    } else {
        for (const auto& i: arguments) {
            i->ex(sList, function, blbl, clbl);
        }
        printf("\tcall\tL%03d, %d\n", functionLabel[i], (int)arguments.size());
    }
}

void CallNode::check(vector<int> & sList, int functionID) {
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
            first->check(sList, functionID);
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
        f->match(arguments, sList, functionID);
        for (const auto & i: arguments) {
            i->check(sList, functionID);
        }
        if (functionLabel.count(i) == 0) {
            functionLabel[i] = lbl++;
            sList.push_back(i);
            f->checkStmt(sList, i);
            sList.pop_back();
        }
    }
}

VarNode::VarNode(int i, bool global, const vector<shared_ptr<ExprNode>>& s):IDNode(i), global(global), subscriptions(s) {;}

void VarNode::pushPop(vector<int>&sList, int functionID, bool push) const {
    string instruction = push ? "push" : "pop";
    if (subscriptions.size() == 0) {
        if (global) {
            cout << "\t" << instruction << "\tsb[" <<  addressTable[GLOBAL].second[GLOBAL][i] << "]\n";
        } else if (functionID == GLOBAL) {
            cout << "\t" << instruction << "\tsb[" << addressTable[GLOBAL].second[getDefinitionScope(sList, GLOBAL)][i] << "]\n";
        } else {
            auto address = addressTable[functionID].second[getDefinitionScope(sList, functionID)][i];
            cout << "\t" << instruction << "\tfp[" << address << "]" << endl;
        }
        return;
    }
    getOffSet(sList, functionID, -1, -1);
    if (global) {
        printf("\tpush\t%d\n", addressTable[GLOBAL].second[GLOBAL][i]);
    } else if (functionID == GLOBAL) {
        auto address = addressTable[GLOBAL].second[getDefinitionScope(sList, functionID)][i];
        printf("\tpush\t%d\n", address);
    } else {
        auto address = addressTable[functionID].second[getDefinitionScope(sList, functionID)][i];        
        printf("\tpush\tfp[%d]\n", address);
        if (address >= 0) {
            printf("\tpush\tfp\n");
            printf("\tadd\n");
        }
    }
    printf("\tadd\n");
    printf("\tpop\tac\n");
    cout << "\t" << instruction << "\tsb[ac]\n";
}

void VarNode::push(vector<int>& sList, int functionID) const {
    pushPop(sList, functionID, true);
}

void VarNode::pop(vector<int>&sList, int functionID) const {
    pushPop(sList, functionID, false);
}

void VarNode::ex(vector<int>& sList, int functionID, int blbl, int clbl) const {
    int definitionScope = getDefinitionScope(sList, functionID);
    if (sizeMap[{definitionScope, i}].size() != 0 and subscriptions.size() == 0) { // pass by reference
        int address = addressTable[functionID].second[definitionScope][i];
        if (address < 0) { // is parameter
            printf("\tpush\tfp[%d]\n", address);
        } else {
            if (global) {
                printf("\tpush\t%d\n", addressTable[GLOBAL].second[GLOBAL][i]);
            } else if (functionID == GLOBAL) {
                auto address = addressTable[GLOBAL].second[getDefinitionScope(sList, functionID)][i];
                printf("\tpush\t%d\n", address);
            } else {
                printf("\tpush\t%d\n", addressTable[functionID].second[definitionScope][i]);
                printf("\tpush\tfp\n");
                printf("\tadd\n");
            }
        }
    } else {
        push(sList, functionID);
    }
}
void VarNode::getOffSet(vector<int>& sList, int functionID, int blbl, int clbl) const {
    auto dimension = sizeMap[{global?GLOBAL:functionID, i}];
    auto size = vector<int>();
    int acc = 1;
    for (auto j = dimension.size() -1 ; j != 0; --j) {
        acc *= dimension[j];
        size.push_back(acc);
    }
    reverse(size.begin(), size.end());
    vector<int> values;
    bool known = true;
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
            known = false;
        }
    }
    vector<int> offsets;
    if (known) {
        for (int i = 0; i != values.size() - 1; ++i) {
            offsets.push_back(size[i] * values[i]);
        }
        offsets.push_back(values.back());
        int sum = accumulate(offsets.begin(), offsets.end(), 0);
        printf("\tpush\t%d\n", sum);
    } else {
        for (int i = 0; i != values.size() - 1; ++i) {
            if (values[i] != -1) {
                printf("\tpush\t%d\n", size[i] * values[i]);
            } else {
                subscriptions[i]->ex(sList, functionID, blbl, clbl);
                printf("\tpush\t%d\n", size[i]);
                printf("\tmul\n");
            }
        }
        if (not subscriptions.back()->isKnown()) {
            subscriptions.back()->ex(sList, functionID, blbl, clbl);
        } else {
            printf("\tpush\t%d\n", subscriptions.back()->getValue());
        }
        for (int i = 0; i != subscriptions.size() - 1; ++i) {
            printf("\tadd\n");
        }
    }
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

void VarNode::declare(vector<int> &sList, int functionID) const {
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
        if (functionID == GLOBAL) {
            cerr << "Refer to global variable " << reverseLookup[i] << " in the global scope" << endl;
            exit(-1);
        }
        if (variableTable[GLOBAL].count(i) != 0) {
            cerr << "Redeclaration of variable " << reverseLookup[i] << endl;
            exit(-1);
        }
        auto address = findSpace(functionID, size);
        if (address == -1) {
            addressTable[GLOBAL].second[GLOBAL][i] = addressTable[GLOBAL].first;
            addressTable[GLOBAL].first += size;
        } else {
            addressTable[GLOBAL].second[GLOBAL][i] = address;
        }
        variableTable[GLOBAL].insert(i);
        sizeMap[{GLOBAL, i}] = dimension;
    } else {
        int s = getDefinitionScope(sList, functionID);
        if (s != NOT_FOUND) {
            cerr << "Redeclaration of variable " << reverseLookup[i] << endl;
            exit(-1);
        }
        variableTable[sList.back()].insert(i);
        #if DEBUG
            cerr << "add variable " << i << " to " << sList.back() << endl;
        #endif
        auto address = findSpace(functionID, size);
        if (address == -1) {
            addressTable[functionID].second[sList.back()][i] = addressTable[functionID].first;
            addressTable[functionID].first += size;
        } else {
            addressTable[functionID].second[sList.back()][i] = address;
        }
        sizeMap[{sList.back(), i}] = dimension;
    }
}

int VarNode::getDimensions() const {
    return subscriptions.size();
}

void VarNode::match() {matched = true;}

void VarNode::assign(vector<int> & sList, int functionID) const {
    if (functionTable.count(i) != 0) {
        cerr << "Redefinition of function: " << reverseLookup[i] << endl;
        exit(-1);
    }
    for (auto & i : subscriptions) {
        i->check(sList, functionID);
        i->evaluate();
    }
    if (global) {
        if (functionID == GLOBAL) {
            cerr << "Refer to global variable " << reverseLookup[i] << " in the global scope" << endl;
            exit(-1);
        }
        if (variableTable[GLOBAL].count(i) != 0 and sizeMap[{GLOBAL, i}].size() != subscriptions.size()) {
            cerr << "Invalid assignment to array " << reverseLookup[i] << endl;
            exit(-1);
        }
        if (variableTable[GLOBAL].count(i) == 0) {
            if (subscriptions.size() != 0) {
                cerr << "Invalid assignment to array " << reverseLookup[i] << endl;
                exit(-1);
            }
            variableTable[GLOBAL].insert(i);
            addressTable[GLOBAL].second[GLOBAL][i] = addressTable[GLOBAL].first++;
            sizeMap[{GLOBAL, i}] = vector<int>();
        }
    } else {
        int s = getDefinitionScope(sList, functionID);
        if (s != NOT_FOUND and sizeMap[{s, i}].size() != subscriptions.size()) {
            cerr << "Invalid assignment to array " << reverseLookup[i] << endl;
            exit(-1);
        }
        if (s == NOT_FOUND) {
            if (subscriptions.size() != 0) {
                cerr << "Invalid assignment to array " << reverseLookup[i] << endl;
                exit(-1);
            }
            auto address = findSpace(functionID, 1);
            if (address == -1) {
                addressTable[functionID].second[sList.back()][i] = addressTable[functionID].first++;
            } else {
                addressTable[functionID].second[sList.back()][i] = address;
            }
            variableTable[sList.back()].insert(i);
            #if DEBUG
                cerr << "add variable " << i << " to " << sList.back() << endl;
            #endif
            sizeMap[{sList.back(), i}] = vector<int>();
        }
    }
}

int VarNode::getDefinitionScope(const vector<int>& sList, int functionID) const {
    if (global) {
        return GLOBAL;
    }
    for (auto j = sList.size() - 1; j != -1; --j) {
        if (variableTable[sList[j]].count(i) != 0) {
            return sList[j];
        }
        if (sList[j] == functionID) {
            return NOT_FOUND;
        }
    }
}

bool  VarNode::isGlobal() const {return global;}

void VarNode::check(vector<int>& sList, int functionID) {
    if (global) {
        if (variableTable[GLOBAL].count(i) == 0) {
            cerr << "Undefined reference to global variable: " << reverseLookup[i] << endl;
            exit(-1);
        }
        if (functionID == GLOBAL) {
            cerr << "Refer to a global variable: " << reverseLookup[i] << " in the global scope" << endl;
            exit(-1);
        }
    } else {
        auto s = getDefinitionScope(sList, functionID);
        if (s == NOT_FOUND) {
            cerr << "Undefined reference to variable " << reverseLookup[i] << endl;
            exit(-1);
        }
    }
    for (auto & i : subscriptions) {
        i->check(sList, functionID);
        i->evaluate();
    }
}

DeclareNode::DeclareNode(const shared_ptr<VarNode>& v, const shared_ptr<ExprNode> e):variable(v), initializer(e) {;}
void DeclareNode::ex(vector<int>& sList, int functionID, int blbl, int clbl) const {
    if (initializer != nullptr) {
        auto dimension = sizeMap[{sList.back(), variable->getID()}];
        int address = addressTable[functionID].second[variable->getDefinitionScope(sList, functionID)][variable->getID()];        
        int size = accumulate(dimension.begin(), dimension.end(), 1, multiplies<int>());
        string base =(variable->isGlobal() or functionID == GLOBAL) ? "sb" : "fp";
        for (int i = 0; i != size; ++i) {
            initializer->ex(sList, functionID, blbl, clbl);
            cout << "\tpop\t" << base << "[" << address + i << "]\n";
        }
    }
}

void DeclareNode::check(vector<int>& sList, int functionID) {
    if (initializer != nullptr) {
        initializer->check(sList, functionID);
    }
    variable->declare(sList, functionID);
}