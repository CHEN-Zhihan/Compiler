#include <iostream>
using std::cout;    using std::endl;
using std::cerr;

#include <cstdio>
using std::printf;

#include <map>
using std::map;

#include <set>
using std::set;

#include "Node.h"
#include "parser.tab.h"


#define ADDSCOPE(node)\
    sList.push_back(++scopeCounter);\
    variableTable[scopeCounter] = set<int>();\
    functionTable[scopeCounter] = map<int, shared_ptr<Node> >();\
    node->check(sList, functionBase);\
    sList.pop_back();

extern int lbl;
extern map<int, string> operatorInstruction;
extern map<int, int> functionLabel;
extern map<int, map<int, int> > addressTable;
extern map<int, map<int, shared_ptr<Node> > > functionTable;
extern map<int, set<int> > variableTable;
extern int GLOBAL;
extern vector<string> reverseLookup;
extern int scopeCounter;

StrNode::StrNode(const string& v):value(v) {;}
void StrNode::ex(int bl, int cl, int func) {
    cout << "\tpush\t" << value << "\n";
}

CharNode::CharNode(const string& v):value(v) {;}
void CharNode::ex(int bl, int cl, int func) {
    cout << "\tpush\t" << value << "\n";
}

IntNode::IntNode(const int& i):value(i){;}
void IntNode::ex(int, int, int) {
    cout << "\tpush\t" << value << "\n";
}

OprNode::OprNode(int oper, const vector<shared_ptr<Node> >& op):oper(oper), op(op) {;}
void OprNode::ex(int blbl, int clbl, int function) {
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
        } case WHILE: {
            lblx = lbl++;
            lbly = lbl++;
            printf("L%03d:\n", lblx);
            op[0]->ex(blbl, clbl, function);
            printf("\tj0\tL%03d\n", lbly);
            op[1]->ex(lbly, lblx, function);
            printf("\tjmp\tL%03d\n", lblx);
            printf("L%03d:\n", lbly);
            break;
        } case IF: {
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
        } case RETURN: {
            if (op.size() == 0) {
                printf("\tpush\t0\n");
            } else {
                op[0]->ex(blbl, clbl, function);
            }
            printf("\tret\n");
            break;
        } case '=': {
            op[1]->ex(blbl, clbl, function);
            if (VarNode* v = dynamic_cast<VarNode*>(op[0].get())) {
                v->pop(function);
            }
            break;
        } case UMINUS: {
            op[0]->ex(blbl, clbl, function);
            printf("\tneg\n");
            break;
        } default: {
            for (const auto& i: op) {
                i->ex(blbl, clbl, function);
            }
            if (operatorInstruction.count(oper) != 0) {
                cout << "\t" << operatorInstruction[oper] << "\n" << endl;
            }
        }
    }
}
void OprNode::check(vector<int>& sList, int functionBase) {
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
        } case UMINUS: {
            op[0]->check(sList, functionBase);
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
        } case '=': {
            op[1]->check(sList, functionBase);
            dynamic_cast<VarNode*>(op[0].get())->assign(sList, functionBase);
            break;
        } default: {
            for (const auto & i: op) {
                i->check(sList, functionBase);
            }
        }
    }
}


IDNode::IDNode(int i):i(i){;}

FunctionNode::FunctionNode(int i,
                           const list<shared_ptr<Node> > parameters,
                           const shared_ptr<Node> & stmts): IDNode(i), parameters(parameters), stmts(stmts){;}

void FunctionNode::ex(int blbl, int clbl, int function) {
    ;
}

void FunctionNode::check(vector<int>& sList, int base) {
    //TODO
}

CallNode::CallNode(int i, const list<shared_ptr<Node> > a):IDNode(i), arguments(a) {;}

void CallNode::ex(int blbl, int clbl, int function) {
    if (i < 3) {
        cout << "\t" << reverseLookup[i] << "\n";
        dynamic_cast<VarNode*>(arguments.front().get())->pop(function);
    } else if (i < 9) {
        arguments.front()->ex(blbl, clbl, function);
        cout << "\t" << reverseLookup[i] << "\n";
    } else {
        for (const auto& i: arguments) {
            i->ex(blbl, clbl, function);
        }
        printf("\tcall\tL%03d, %d\n", functionLabel[i], (int)arguments.size());
    }
}

void CallNode::check(vector<int> & sList, int functionBase) {
    //TODO
}

VarNode::VarNode(int i, bool global):IDNode(i), global(global){;}

void VarNode::push(int function) {
    if (global or function == GLOBAL) {
        printf("\tpush\tsb[%d]\n", addressTable[GLOBAL][i]);
    } else {
        printf("\tpop\tfp[%d]\n", addressTable[function][i]);
    }
}

void VarNode::pop(int function) {
    if (global or function == GLOBAL) {
        printf("\tpop\tsb[%d]\n", addressTable[GLOBAL][i]);
    } else {
        printf("\tpop\tfp[%d]\n", addressTable[function][i]);
    }
}

void VarNode::ex(int blbl, int clbl, int function) {
    push(function);
}

void VarNode::check(vector<int>& sList, int functionBase) {
    //TODO;
}