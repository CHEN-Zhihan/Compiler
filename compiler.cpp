#include <iostream>
using std::cout;    using std::endl;
using std::printf;  using std::cerr;
#include <string>
using std::string;
#include <list>
using std::list;

#include <vector>
using std::vector;

#include <map>
using std::map; using std::pair;

#include <set>
using std::set;

#include "Node.h"
#include "parser.tab.h"

int lbl;


using id = int;
using scope = int;
using functionID = int;
using operatorID = int;

#define ADDSCOPE(node)\
    sList.push_back(++scopeCounter);\
    variableTable[scopeCounter] = set<id>();\
    functionTable[scopeCounter] = map<functionID, nodeType *>();\
    preCheck(node, sList, functionBase);\
    sList.pop_back();



scope GLOBAL = 0;
constexpr int VALUE_CHECK = 1;
constexpr int REFERENCE_CHECK = 2;

#define DEBUG true

//extern vector<string> reverseLookup;/* maps a variable identifier to a name*/
map<scope, map<functionID,  shared_ptr<Node> > > functionTable;
map<scope, set<id> > variableTable;
map<int, string> operatorInstruction;
/*
static set<operatorID> comparator;/*determine whether an operator is a comparator. If yes, the resulting type should be BOOL
static map<operatorID, string> reverseSymbolLookup; /*maps an operator to its name
static map<id, set<pair<valueEnum, valueEnum> > > applicable; /*maps an operator to the set of operands it can perform on.
static vector<string> reverseTypeLookup; /*maps an valueEnum value to its name.
static vector<valueEnum> requiredType; /*maps a variable identifier to the type of arguments. Used for built-in functions.*/

static scope scopeCounter;
static set<functionID> functionChecked;
map<functionID, int> functionLabel;
map<functionID, map<id, int> > addressTable;
/*
int ex(nodeType*, int, int, functionID);

scope getDefinitionScope(id target, const vector<scope>& sList, int functionBase) {
    for (auto i = sList.size() - 1; i != functionBase - 1; --i) {
        if (variableTable[sList[i]].count(target) != 0) {
            return sList[i];
        }
    }
    return variableTable[GLOBAL].count(target) != 0? GLOBAL:-1;
}

void preAssign(idNodeType& node, const vector<scope>& sList, int functionBase) {
    int variable = node.i;
    if (functionTable[GLOBAL].count(variable) != 0) {
        auto temp = functionTable[GLOBAL][variable];
        if (!temp or temp->id.function.origin == temp->id.i) {
            cerr << "Redefinition of function " << reverseLookup[variable] << endl;
            exit(-1);
        }
    }
    if (node.type == VAR and node.global) {
        if (variableTable[GLOBAL].count(variable) == 0) {
            auto size = addressTable[GLOBAL].size();
            variableTable[GLOBAL].insert(variable);
            addressTable[GLOBAL][variable] = size;
        }
    } else {
        auto s = getDefinitionScope(variable, sList, functionBase);
        if (s == -1) {
            if (node.type == VAR and addressTable[sList[functionBase]].count(variable) == 0) {
                auto size = addressTable[sList[functionBase]].size();
                if (sList[functionBase] != GLOBAL) {
                    size -= functionTable[GLOBAL][sList[functionBase]]->id.function.parameters->size();
                }
                addressTable[sList[functionBase]][variable] = size;
                variableTable[sList.back()].insert(variable);
                #if DEBUG
                    cerr << "add variable " << variable << " to " << sList.back() << endl;
                #endif
            }
        }
    }
}


void defineFunctions() {
    for (const auto& FID : functionLabel) {
        nodeType * func = functionTable[GLOBAL][FID.first];
        func = functionTable[GLOBAL][func->id.function.origin];
        int tempVariables = int(variableTable[FID.first].size() - func->id.function.parameters->size());
        printf("L%03d:\n", functionLabel[FID.first]);
        if (tempVariables != 0) {
            printf("\tpush\t%d\n", tempVariables);
            printf("\tpush\tsp\n");   
            printf("\tadd\n");
            printf("\tpop\tsp\n");
        }
        ex(func->id.function.stmts, 998, 998, FID.first);
        printf("\tpush\t0\n");
        printf("\tret\n");
    }
}

void preCheck(nodeType * & p, vector<scope>& sList, int functionBase) {
    if (p == nullptr) {
        return;
    }
    switch (p->node) {
        case nodeCon: {
            break;
        } case nodeId: {
            idNodeType& ID = p->id;
            auto VID = ID.i;
            switch (ID.type) {
                case FUNCTION: {
                    if (sList.back() != GLOBAL) {
                        cerr << "Function " << reverseLookup[VID] << " can only be defined in the global scope" << endl;
                        exit(-1);
                    }
                    if (variableTable.count(ID.i) == 0) {
                        variableTable[ID.i] = set<int>();
                        addressTable[ID.i] = map<id, int>();
                        int size = 0;
                        for (auto i = ID.function.parameters->begin(); i != ID.function.parameters->end(); ++i) {
                            if (functionTable[GLOBAL].count((*i)->id.i) != 0) {
                                cerr << "Redefinition of function: " << reverseLookup[(*i)->id.i] << endl;
                                exit(-1);
                            }
                            variableTable[ID.i].insert((*i)->id.i);
                            #if DEBUG
                                cerr << "add variable " << (*i)->id.i << " to " << ID.i << endl;
                            #endif
                            addressTable[ID.i][(*i)->id.i] = - 3 + (size++) - ID.function.parameters->size();
                        }
                        functionTable[GLOBAL][ID.i] = p;
                        #if DEBUG
                            cerr << "adding function " << ID.i << " to " << GLOBAL << endl;
                        #endif
                        variableTable[GLOBAL].insert(ID.i);
                    }
                    break;
                } case VAR: {
                    if (ID.global) {
                        if (variableTable[GLOBAL].count(ID.i) == 0) {
                            cerr << "Undefined reference to global variable: " << reverseLookup[ID.i] << endl;
                            exit(-1);
                        }
                        if (sList.back() == GLOBAL) {
                            cerr << "Refer to a global variable: " << reverseLookup[ID.i] << " in the global scope" << endl;
                            exit(-1);
                        }
                    } else {
                            auto s = getDefinitionScope(ID.i, sList, functionBase);
                        if ((s == -1) || (s == 0 and sList[functionBase] != 0)) {
                            cerr << "Undefined reference to variable " << reverseLookup[p->id.i] << endl;
                            exit(-1);
                        }
                    }
                    break;
                } case CALL: {
                    auto FID = p->id.i;
                    if (FID < 9) {
                        if (ID.call.arguments->size() != 1) {
                            cerr << reverseLookup[FID] << " expects 1 argument, got " << ID.call.arguments->size() << endl;
                            exit(-1); 
                        }
                        auto first = ID.call.arguments->front();                    
                        if (FID < 3) {
                            if (first->node != nodeId) {
                                cerr << reverseLookup[FID] << " got invalid argument" << endl;
                                exit(-1);
                            }
                            preAssign(first->id, sList, functionBase);
                        } else {
                            preCheck(first, sList, functionBase);
                        }
                    } else {
                        scope s = getDefinitionScope(FID, sList, functionBase);
                        if (s == -1) {
                            cerr << "Call to undefined function: " << reverseLookup[FID] << endl;
                            exit(-1);
                        }
                        if (functionTable[s].count(FID) == 0) {
                            cerr << "Call to undefined function: " << reverseLookup[FID] << endl;
                            exit(-1);
                        }
                        auto target = functionTable[s][FID];
                        if (target->id.i != target->id.function.origin and s == GLOBAL and sList[functionBase] != GLOBAL) {
                            cerr << "Call to undefined function: " << reverseLookup[FID] << endl;
                            exit(-1);
                        }
                        nodeType* temp = new nodeType(*target);
                        for (auto k = p->id.call.arguments->begin(); k != p->id.call.arguments->end(); k++) {
                            temp->id.function.arguments->push_back(*k);
                        }
                        if (temp->id.function.parameters->size() < temp->id.function.arguments->size()) {
                            cerr << reverseLookup[ID.i] << " expects " << temp->id.function.parameters->size() -  temp->id.function.arguments->size()
                             +p->id.call.arguments->size() << " argument(s), got " << p->id.call.arguments->size() << endl;
                            exit(-1);
                        }
                        for (auto & i : *ID.call.arguments) {
                            preCheck(i, sList, functionBase);
                        }
                        p->id.call.origin = temp->id.function.origin;
                        if (temp->id.function.parameters->size() == temp->id.function.arguments->size()) {
                            if (functionLabel.count(temp->id.function.origin) == 0) {
                                functionLabel[temp->id.function.origin] = lbl++;
                                #if DEBUG
                                    cerr << "adding " << temp->id.function.origin << " to functionLabel" << endl;
                                #endif
                            }
                            if (temp->id.i != sList[functionBase] and functionChecked.count(temp->id.i) == 0) {
                                sList.push_back(temp->id.function.origin);
                                preCheck(temp->id.function.stmts, sList, sList.size() - 1);
                                sList.pop_back();
                                functionChecked.insert(temp->id.function.origin);
                            }
                            p->id.call.arguments = temp->id.function.arguments;
                        } else if (ID.call.arguments->size() != 0) {
                            p = temp;
                        } else {
                            cerr << reverseLookup[ID.i] << " expects " << temp->id.function.parameters->size() - temp->id.function.arguments->size()
                             << " argument(s), got " << ID.call.arguments->size() << endl;
                            exit(-1);
                        }
                    }
                }
            }
            break;
        } case nodeOpr: {
            switch (p->opr.oper) {
                case BREAK: {
                    break;
                } case CONTINUE: {
                    break;
                } case RETURN: {
                    if (functionBase == 0) {
                        cerr << "return occurs in the global scope" << endl;
                        exit(-1);
                    }
                    nodeType * node = functionTable[GLOBAL][sList[functionBase]];
                    if (p->opr.nops != 0) {
                        preCheck(p->opr.op[0], sList, functionBase);
                    }
                    break;
                } case UMINUS: {
                    preCheck(p->opr.op[0], sList, functionBase);
                    break;
                } case FOR: {
                    sList.push_back(scopeCounter++);
                    preCheck(p->opr.op[0], sList, functionBase);
                    preCheck(p->opr.op[1], sList, functionBase);
                    preCheck(p->opr.op[3], sList, functionBase);
                    preCheck(p->opr.op[2], sList, functionBase);
                    sList.pop_back();
                    break;
                } case WHILE: {
                    preCheck(p->opr.op[0], sList, functionBase);
                    ADDSCOPE(p->opr.op[1]);
                    break;
                } case IF: {
                    preCheck(p->opr.op[0], sList, functionBase);
                    ADDSCOPE(p->opr.op[1]);
                    if (p->opr.nops > 2) {
                        ADDSCOPE(p->opr.op[2]);
                    }
                    break;
                } case '=': {
                    preCheck(p->opr.op[1], sList, functionBase);
                    if (p->opr.op[1]->node == nodeCon or p->opr.op[1]->node == nodeOpr) {
                        preAssign(p->opr.op[0]->id, sList, functionBase);
                    } else {
                        preAssign(p->opr.op[0]->id, sList, functionBase);
                        scope s = getDefinitionScope(p->opr.op[1]->id.i, sList, functionBase);
                        if (p->opr.op[1]->id.type == FUNCTION) {
                            p->opr.op[0]->id.type = p->opr.op[1]->id.type;                        
                            p->opr.op[0]->id.function = p->opr.op[1]->id.function;
                            functionTable[sList.back()][p->opr.op[0]->id.i] = p->opr.op[0];
                            #if DEBUG
                                cerr << "adding " << p->opr.op[0]->id.i << " to " << sList.back() << endl;
                            #endif
                        }
                    }
                    break;
                } default: {
                    if (p->opr.nops == 2 and p->opr.op[0] != nullptr and p->opr.op[1] != nullptr) {
                        preCheck(p->opr.op[0], sList, functionBase);
                        preCheck(p->opr.op[1], sList, functionBase);
                    }
                    break;
                }
            }
        }
    }
}
*/
void run(shared_ptr<Node> p) {
    operatorInstruction = map<int, string> {{'+', "add"}, {'-', "sub"}, {'*', "mul"}, 
                                            {'/', "div"}, {'%', "mod"}, {'<', "compLT"},
                                            {'>', "compGT"}, {GE, "compGE"}, {LE, "compLE"},
                                            {NE, "compNE"}, {EQ, "compEQ"}, {AND, "and"},
                                            {OR, "or"}};
    variableTable[GLOBAL] = set<id>{};
    functionTable[GLOBAL] = map<functionID, shared_ptr<Node> >();    
    for (auto i = 0; i != 9; ++i) {
 //       functionTable[GLOBAL][i] = nullptr;
    }
    scopeCounter = 1;
    vector<scope> sList{GLOBAL};
    #if DEBUG
//        for (auto i = 0; i != reverseLookup.size(); ++i) {
 //           cerr << i << reverseLookup[i] << endl;
  //      }
    #endif
 //   preCheck(p, sList, GLOBAL);

    if (addressTable[GLOBAL].size() != 0) {
        printf("\tpush\tsp\n");
        printf("\tpush\t%d\n", int(addressTable[GLOBAL].size()));
        printf("\tadd\n");
        printf("\tpop\tsp\n");
    }
 //   ex(p, 998, 998, GLOBAL);
    p->ex(998, 998, GLOBAL);
    printf("\tend\n");
//    defineFunctions();
}

